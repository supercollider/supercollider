

+Pattern {
	
	lock { arg n=1;
		^Pfin(n.asStream, this.asStream)
	}
	//lim { arg mindur, maxdur;
//		^Plim(this, mindur, maxdur)
//	}

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
	

//	scrubForward { arg by, tolerance=0, rate, silent=true;
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
		^quant.nextTimeOnGrid(this) - this.beats
	}
}

