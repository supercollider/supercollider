

+Pattern {
	
	loop { ^Pn(this, inf) }

	repeat { arg repeats = inf; ^Pn(this, repeats) }
	lock { arg n=1;
		^Pfin(n.asStream, this.asStream)
	}
	//lim { arg mindur, maxdur;
//		^Plim(this, mindur, maxdur)
//	}

}

+Stream {
	loop { 
		^FuncStream.new({ arg inval;
			var outval;
			outval = this.next(inval);
			if(outval.isNil) {  this.reset; outval = this.next(inval) };
			outval
		}, { this.reset })  
	}
	repeat { arg repeats = inf;
		var n;
		if(inf === repeats) { ^this.loop };
		n = repeats - 1;
		^FuncStream.new({ arg inval;
			var outval;
			outval = this.next(inval);
			if(n > 0) {
					if(outval.isNil) {  
						n = n - 1; 
						this.reset;
						outval = this.next(inval) 
					};
					outval
			} {Ênil }
		}, {	n = repeats; this.reset })
	}
	
}

+Stream {
	
	fastForward { arg by, tolerance=0;
		var t=0, delta=0, event, scrub;
		if(by <= 0) { ^0.0 };
		while { t.roundUp(tolerance) < by } 
		{ 
			event = this.next(Event.default);
			if(event.isNil) { ("end of stream. Time left:" + (by - t)).inform; ^t - by };
			event = event.copy.put(\freq, \rest);
			delta = event.play;
			if(delta.isNil) { ("end of stream. Time left:" + (by - t)).inform; ^t - by };
			t = t + delta;

		};
		^t - by // time left to next event
	
	}
	

//	scrubForward {Êarg by, tolerance=0, rate, silent=true;
//		var ref;
//		ref = Ref.new;
//		{ ref.value = this.fastForward( by, tolerance=0, rate, silent) }.fork;
//		^ref
//	}
//
	//fastForward { arg by, tolerance=0, rate, silent=true;
//		var t=0, delta=0, event, scrub;
//		if(by <= 0) { ^this };
//		scrub = rate.notNil;
//		
//		while { t.roundUp(tolerance) < by } 
//		{ 
//			event = this.next(Event.default);
//			if(event.isNil) { ("end of stream. Time left:" + (by - t)).inform; ^t - by };
//			if(silent) {  
//				event = event.copy.put(\freq, \rest) 
//			};
//			
//			if(scrub) { 
//				event = event.copy.put(\dur, event[\dur] / rate)
//			};
//						
//			delta = event.play;
//			if(delta.isNil) { ("end of stream. Time left:" + (by - t)).inform; ^t - by };
//			
//			if(scrub) { 
//				delta.wait; 
//				delta = delta * rate; 
//			};
//			t = t + delta;
//
//		};
//		^t - by // time ahead
//	
//	}

	
}

+Object {

	loop {}
	
	repeat { arg repeats = inf; 
		^if(inf === repeats) { this } {
			Pn(this, repeats).asStream 
		}
	}
}

+Nil {
	repeat {}
}


+UGen {

	lag { arg lagTime=0.1;
		^Lag.multiNew(this.rate, this, lagTime)
	}
	lag2 { arg lagTime=0.1;
		^Lag2.multiNew(this.rate, this, lagTime)
	}
	lag3 { arg lagTime=0.1;
		^Lag3.multiNew(this.rate, this, lagTime)
	}

}

+SequenceableCollection {
	lag { arg lagTime=0.1;
		^this.collect({ arg item; item.lag(lagTime) })
	}
	lag2 { arg lagTime=0.1;
		^this.collect({ arg item; item.lag2(lagTime) })
	}
	lag3 { arg lagTime=0.1;
		^this.collect({ arg item; item.lag3(lagTime) })
	}
}


+ EventStreamPlayer {
	
	xplay { arg fadeTime, argClock, doReset = false, quant=1.0;
		if (doReset, { this.reset });
		clock = argClock ? clock ? TempoClock.default;
		stream = PfadeIn(originalStream, fadeTime).asStream;
		clock.play(this, quant);
	}
	xstop { arg fadeTime;
		stream = PfadeOut(stream, fadeTime).asStream;
	}


}
+PauseStream {

	xplay { arg fadeTime, argClock, doReset = false, quant=1.0;
		this.play(argClock, doReset, quant);
	}
	xstop { // stop after fade?
		this.stop;
	}
}

+TempoClock {
	timeToNextBeat { arg quant=0.0;
		var t;
		t = this.elapsedBeats;
		^quant.nextTimeOnGrid(this) - t
		//^t.roundUp(quant) - t
	}
}

