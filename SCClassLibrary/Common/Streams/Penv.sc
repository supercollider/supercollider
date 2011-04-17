Penv : Env {
	embedInStream { arg inval;
		var startTime;
		startTime = thisThread.endBeat ? thisThread.beats;
		thisThread.endBeat = this.times.sum + startTime;
		while
			{ thisThread.beats < thisThread.endBeat }
			{ inval = yield(this.at(thisThread.beats - startTime))};
		^inval
	}
}
