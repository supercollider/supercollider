


Tdef : Task {
	
	*new { arg key, func, clock;
		var res;
		res = this.at(key);
		if(res.isNil, { 
				res = super.new(func, clock);
				this.put(key, res)
		}, {
			if(func.notNil, {
				res.stream = Routine(func)
			});
		})
		^res
	}
	*at { arg key;
		^Library.at(this, key)
	}
	*put { arg key, obj;
		Library.put(this, key, obj)
	}

}

Pdef : EventStreamPlayer {
	
	classvar <all, <>defaultEvent, <defaultStream;
	var isPlaying=false, <>quant=1.0;
	
	
	
	*new { arg key, pat, event;
		var res, stream;
		res = this.at(key);
		stream = pat.asStream;
		if(res.isNil, {
				res = super.new(stream ? this.defaultStream, event ? this.defaultEvent);
				this.put(key, res)
		}, {
			if(stream.notNil, { res.stream = stream });
			if(event.notNil, { res.event = event });
		});
		^res
	}
	*at { arg key;
		^all.at(key)
	}
	*put { arg key, obj;
		all.put(key, obj)
	}
	*initClass {
		defaultStream = Pbind(\freq, \rest).asStream;
		CmdPeriod.add(this);
		this.clear;
	}
	
	*cmdPeriod { all.do({ arg item; item.stop }) }
	
	*remove { arg key;
		all.removeAt(key).stop;
	}
	*removeAll { all.do({ arg item; item.stop }); this.clear }
	*clear { all = () }
	
	
		
	isPlaying { ^isPlaying } // override superclass 
	
	stream_ { arg str;
		if(clock.isKindOf(TempoClock), { 
			clock.play({ 	
				this.prSetStream(str); nil 
			}, 1) 
		}, {
				this.prSetStream(str);
		});
	}
	
	play { arg argClock, doReset = false, argQuant;
		if (isPlaying, { "alrready playing".postln; ^this });
		super.play(argClock, doReset, argQuant ? quant);
		isPlaying = true;
	}
	
	stop {  stream = nil; isPlaying = false; }
	pause { this.stop }
	
	// use like a stream
	
	asStream {
		^Routine.new({ arg inval;
			var val;
			this.refresh;
			while({
				val = stream.next(inval);
				val.notNil;
			}, {
				val.yield;
			})
		
		})
	}
	
	embedInStream { arg stream;
		^this.asStream.embedInStream(stream)
	}
	
	collect { arg func;
		^this.asStream.collect(func)
	}
	
	// private 
	prSetStream { arg str;
			var old;
			old = stream;
			super.stream = str;
			if(isPlaying and: { old.isNil }, { isPlaying = false; this.play });
	}
	
}




