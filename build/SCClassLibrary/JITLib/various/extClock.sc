+TempoClock {
	
	schedToBeat { arg delta, item;
		var seconds, beats;
		seconds = thisThread.seconds + delta;
		beats = this.secs2beats(seconds);
		this.schedAbs(beats.ceil, item)
	} 

}

+SystemClock {
	
	schedToBeat { arg delta, item;
		this.sched(delta, item)
	}
	*schedToBeat { arg delta, item;
		this.sched(delta, item)
	} 

}