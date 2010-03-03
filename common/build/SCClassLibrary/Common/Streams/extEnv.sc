//As a pattern, an Env defines a function of time sampled at the time positions when it is called.
//Sample functions of time can describe attributes such as dynamics, chord progressions, etc that occur over time but can be articulated with different rhythmic patterns.


+Env {
	embedInStream { arg inval;
		var startTime;
		startTime = thisThread.endBeat ? thisThread.beats;
		thisThread.endBeat = this.times.sum + startTime;
		loop {
			inval = yield(this.at(thisThread.beats - startTime));
		}
	}
	
	asStream {
		^Routine({ arg inval; this.embedInStream(inval) })
	}

	asSignal { arg length = 400;
		var duration, signal, ratio;
		duration = times.sum;
		ratio = duration/(length - 1);
		signal = Signal(length);
		length.do({ arg i; signal.add(this.at(i * ratio)) });
		^signal;
	}

	plot { arg size = 400, bounds, minval, maxval, parent;
		this.asSignal(size).plot(bounds: bounds, minval: minval, maxval: maxval, parent: parent);
	}
}
