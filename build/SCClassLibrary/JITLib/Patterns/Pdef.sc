StreamPlayerReference {
	
	var <>quant=1.0, <>isPlaying=false;
	var <player;
	

	*new { arg key ... argList;
		var res, stream;
		res = this.at(key);
		if(res.isNil, {
				res = super.new.initPlayer(argList);
				this.put(key, res)
		}, {
			res.setProperties(argList);
		});
		^res
	}
	
	play { arg argClock, doReset = false, argQuant;
		if (isPlaying, { "reference already playing".postln; ^this });
		player.play(argClock, doReset, argQuant ? quant);
		isPlaying = true;
	}
	
	clock { ^player.clock }
	stop {  player.stop; isPlaying = false; }
	pause { this.stop }
	
	*put { this.subclassResponsibility(thisMethod) }
	*at { this.subclassResponsibility(thisMethod) }
	setProperties { this.subclassResponsibility(thisMethod) }
	initPlayer { this.subclassResponsibility(thisMethod) }
	
	sched { arg func;
		var clock;
		clock = this.clock;
		if(clock.isKindOf(TempoClock), { 
			clock.play({ 	
				func.value; nil 
			}, quant) 
		}, func);
	}

}


Tdef : StreamPlayerReference {
	classvar <>all;
	
	*initClass {
		CmdPeriod.add(this);
		this.clear;
	}
	*cmdPeriod { this.all.do({ arg item; item.stop }) }
	
	*remove { arg key;
		this.all.removeAt(key).stop;
	}
	*removeAll { this.all.do({ arg item; item.stop }); this.clear }
	*clear { all = () }
	*at { arg key;
		^this.all.at(key)
	}
	*put { arg key, obj;
		this.all.put(key, obj)
	}
	initPlayer { arg argList;
		var func;
		#func = argList;
		player = Task.new(func ? { 1.yield });
	}
	
	setProperties { arg argList;
		var func;
		#func = argList;
		if(func.notNil, { 
			this.stream = Routine.new(func);
		})
	}
	 
	stream_ { arg str;
			if(isPlaying)
				{ 
					if(player.isPlaying)
					 { this.sched({ player.stream = str })  }
			 		 { player.stream = str; player.play(quant: quant) }
				}
			 	{ 
			 		player.stream = str
			 	}
	}


}

Pdef : Tdef {
	classvar <all, <>defaultEvent, <defaultStream;
	
	*initClass {
		CmdPeriod.add(this);
		this.clear;
		defaultStream = Pbind(\freq, \rest).asStream;
	}
	*clear { all = () }
	
	initPlayer { arg argList;
		var pat, event, stream;
		#pat, event = argList;
		player = EventStreamPlayer.new(
			stream.asStream ? this.class.defaultStream, 
			event ? this.class.defaultEvent
		);
	}
	
	setProperties { arg argList;
		var pat, event, stream;
		#pat, event = argList;
		stream = pat.asStream;
		if(event.notNil, { player.event = event });
		if(stream.notNil, { this.stream = stream });
	}
	
	event_ { arg inEvent; player.event = inEvent }
	
	// use like a stream
	
	asStream {
		^Routine.new({ arg inval;
			var val;
			player.refresh;
			while({
				val = player.stream.next(inval);
				val.notNil;
			}, {
				val.yield;
			})
		
		})
	}
	
	mute { player.mute }
	unmute { player.unmute }
	
	embedInStream { arg stream;
		^this.asStream.embedInStream(stream)
	}
	
	collect { arg func;
		^this.asStream.collect(func)
	}
	
}

