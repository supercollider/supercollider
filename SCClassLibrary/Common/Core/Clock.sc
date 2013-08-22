// clocks for timing threads.

Clock {
	// abstract class
	*play { | task | this.sched(0, task) }
	*seconds { ^thisThread.seconds }

	// tempo clock compatibility
	*beats { ^thisThread.seconds }
	*beats2secs { | beats | ^beats }
	*secs2beats { | secs | ^secs }
	*beats2bars { ^0 }
	*bars2beats { ^0 }
	*timeToNextBeat { ^0 }
	*nextTimeOnGrid { | quant = 1, phase = 0|
		if (quant ==0) { ^this.beats + phase };
		if (phase < 0) { phase = phase % quant };
		^roundUp(this.beats - (phase % quant), quant) + phase;
	}
}

SystemClock : Clock {
	*clear {
		var queue = thisProcess.prSchedulerQueue;
		if (queue.size > 1) {
			forBy(1, queue.size-1, 3) {|i|
				queue[i+1].removedFromScheduler
			};
		};
		this.prClear;
	}
	*sched { arg delta, item;
		_SystemClock_Sched
		^this.primitiveFailed
	}
	*schedAbs { arg time, item;
		_SystemClock_SchedAbs
		^this.primitiveFailed
	}
	*prClear {
		_SystemClock_Clear
		^this.primitiveFailed
	}
}

AppClock : Clock {
	classvar scheduler;
	*initClass {
		scheduler = Scheduler.new(this, drift:true, recursive:false);
	}
	*clear {
		scheduler.clear;
	}
	*sched { arg delta, item;
		scheduler.sched(delta, item);
		this.prSchedNotify;
	}
	*tick {
		var saveClock = thisThread.clock;
		thisThread.clock = this;
		scheduler.seconds = Main.elapsedTime;
		thisThread.clock = saveClock;
		^scheduler.queue.topPriority;
	}
	*prSchedNotify {
		// notify clients that something has been scheduled
		_AppClock_SchedNotify
	}
}

Scheduler {
	var clock, drift, <>recursive, beats = 0.0, <seconds = 0.0, <queue, expired, wakeup;

	*new { arg clock, drift = false, recursive=true;
		^super.newCopyArgs(clock, drift, recursive).init;
	}
	init {
		beats = thisThread.beats;
		queue = PriorityQueue.new;
		expired = Array.new(8);
		wakeup = { |item|
			var delta;
			try {
				delta = item.awake( beats, seconds, clock );
				if (delta.isNumber) { this.sched(delta, item) };
			} { |error|
				Error.handling = true;
				if (Error.debug) { error.inspect } { error.reportError };
				Error.handling = false;
			}
		};
	}

	play { arg task;
		this.sched(0, task)
	}

	schedAbs { | time, item |
		queue.put(time, item);
	}

	sched { | delta, item |
		var fromTime;
		if (delta.notNil, {
			fromTime = if (drift, { Main.elapsedTime },{ seconds });
			queue.put(fromTime + delta, item);
		});
	}
	clear {
		queue.do {|x| x.removedFromScheduler };
		queue.clear
	}

	isEmpty { ^queue.isEmpty }

	advance { | delta |
		this.seconds = seconds + delta;
	}

	seconds_ { | newSeconds |
		if( recursive ) {
			while {
				seconds = queue.topPriority;
				seconds.notNil and: { seconds <= newSeconds }
			} {
				beats = clock.secs2beats(seconds);
				wakeup.(queue.pop);
			}
		} {
			// First pop all the expired items and only then wake
			// them up, in order for control to return to the caller
			// before any tasks scheduled as a result of this call are
			// awaken.

			while {
				seconds = queue.topPriority;
				seconds.notNil and: { seconds <= newSeconds }
			} {
				expired = expired.add(seconds);
				expired = expired.add(queue.pop);
			};

			expired.pairsDo { | time, item |
				seconds = time;
				beats = clock.secs2beats(time);
				wakeup.(item);
			};
			expired.extend(0); // clear
		};

		seconds = newSeconds;
		beats = clock.secs2beats(newSeconds);
	}
}

TempoClock : Clock {
	classvar <all, <>default;

	var <queue, ptr;

	var <beatsPerBar=4.0, barsPerBeat=0.25;
	var <baseBarBeat=0.0, <baseBar=0.0;
	var <>permanent=false;

/*
You should only change the tempo 'now'. You can't set the tempo at some beat in the future or past, even though you might think so from the methods.

There are several ideas of now:
	elapsed time, i.e. "real time"
	logical time in the current time base.
	logical time in another time base.

logical time is time that is incremented by exact amounts from the time you started. It is not affected by the actual time your task gets scheduled, which may shift around somewhat due to system load. By calculating using logical time instead of actual time, your process will not drift out of sync over long periods. every thread stores a clock and its current logical time in seconds and beats relative to that clock.

elapsed time is whatever the system clock says it is right now. elapsed time is always advancing. logical time only advances when your task yields or returns.

*/

	*new { arg tempo, beats, seconds, queueSize=256;
		^super.new.init(tempo, beats, seconds, queueSize)
	}

	*initClass {
		default = this.new(queueSize: 2048).permanent_(true);
		CmdPeriod.add(this);
	}

	*cmdPeriod {
		all.do({ arg item; item.clear(false) });
		// copy is important: You must never iterate over the same
		// collection from which you're removing items
		all.copy.do({ arg item; if (item.permanent.not, { item.stop })  })
	}

	init { arg tempo, beats, seconds, queueSize;
		queue = Array.new(queueSize);
		this.prStart(tempo, beats, seconds);
		all = all.add(this);
	}

	stop {
		this.changed(\stop);
		this.releaseDependants;
		all.take(this);
		this.prStop;
	}

	play { arg task, quant = 1;
		this.schedAbs(quant.nextTimeOnGrid(this), task)
	}

	playNextBar { arg task; this.schedAbs(this.nextBar, task) }

	tempo {
		_TempoClock_Tempo
		^this.primitiveFailed
	}
	beatDur {
		_TempoClock_BeatDur
		^this.primitiveFailed
	}
	elapsedBeats {
		_TempoClock_ElapsedBeats
		^this.primitiveFailed
		/* primitive does this:
			^this.secs2beats(Main.elapsedTime).
		*/
	}
	beats {
		// returns the appropriate beats for this clock from any thread
		_TempoClock_Beats
		^this.primitiveFailed
		/* primitive does this:
			if (thisThread.clock == this) { ^thisThread.beats }
			^this.secs2beats(thisThread.seconds)
		*/
	}

	seconds { ^thisThread.seconds }

	sched { arg delta, item;
		_TempoClock_Sched
		^this.primitiveFailed
	}
	schedAbs { arg beat, item;
		_TempoClock_SchedAbs
		^this.primitiveFailed
	}
	clear { | releaseNodes = true |
		// flag tells EventStreamPlayers that CmdPeriod is removing them, so
		// nodes are already freed
		// NOTE: queue is an Array, not a PriorityQueue, but it's used as such internally. That's why each item uses 3 slots.
		if (queue.size > 1) {
			forBy(1, queue.size-1, 3) {|i|
				queue[i+1].removedFromScheduler(releaseNodes)
			};
		};
		^this.prClear;
	}


	// for setting the tempo at the current logical time
	// (even another TempoClock's logical time).
	tempo_ { arg newTempo;
		this.setTempoAtBeat(newTempo, this.beats);
		this.changed(\tempo);  // this line is added
	}
	beatsPerBar_ { arg newBeatsPerBar;
		if (thisThread.clock != this) {
			"should only change beatsPerBar within the scheduling thread.".error;
			^this
		};
		this.setMeterAtBeat(newBeatsPerBar, thisThread.beats);
	}

	// for setting the tempo at the current elapsed time .
	etempo_ { arg newTempo;
		this.setTempoAtSec(newTempo, Main.elapsedTime);
	}

	beats2secs { arg beats;
		_TempoClock_BeatsToSecs
		^this.primitiveFailed
	}
	secs2beats { arg secs;
		_TempoClock_SecsToBeats
		^this.primitiveFailed
	}

	prDump {
		_TempoClock_Dump
		^this.primitiveFailed
	}

	nextTimeOnGrid { arg quant = 1, phase = 0;
		if (quant == 0) { ^this.beats + phase };
		if (quant < 0) { quant = beatsPerBar * quant.neg };
		if (phase < 0) { phase = phase % quant };
		^roundUp(this.beats - baseBarBeat - (phase % quant), quant) + baseBarBeat + phase
	}

	timeToNextBeat { arg quant=1.0; // logical time to next beat
		^quant.nextTimeOnGrid(this) - this.beats
	}

	beats2bars { arg beats;
		^(beats - baseBarBeat) * barsPerBeat + baseBar;
	}
	bars2beats { arg bars;
		^(bars - baseBar) * beatsPerBar + baseBarBeat;
	}
	bar {
		// return the current bar.
		^this.beats2bars(this.beats).floor;
	}
	nextBar { arg beat;
		// given a number of beats, determine number beats at the next bar line.
		if (beat.isNil) { beat = this.beats };
		^this.bars2beats(this.beats2bars(beat).ceil);
	}
	beatInBar {
		// return the beat of the bar, range is 0 to < t.beatsPerBar
		^this.beats - this.bars2beats(this.bar)
	}

	// PRIVATE
	prStart { arg tempo;
		_TempoClock_New
		^this.primitiveFailed
	}
	prStop {
		_TempoClock_Free
		^this.primitiveFailed
	}
	prClear {
		_TempoClock_Clear
		^this.primitiveFailed
	}
	setTempoAtBeat { arg newTempo, beats;
		_TempoClock_SetTempoAtBeat
		^this.primitiveFailed
	}
	setTempoAtSec { arg newTempo, secs;
		_TempoClock_SetTempoAtTime
		^this.primitiveFailed
	}
	// meter should only be changed in the TempoClock's thread.
	setMeterAtBeat { arg newBeatsPerBar, beats;
		// bar must be integer valued when meter changes or confusion results later.
		baseBar = round((beats - baseBarBeat) * barsPerBeat + baseBar, 1);
		baseBarBeat = beats;
		beatsPerBar = newBeatsPerBar;
		barsPerBeat = beatsPerBar.reciprocal;
		this.changed(\meter);
	}

	// these methods allow TempoClock to act as TempoClock.default
	*stop { TempoClock.default.stop	}
	*play { | task, quant | TempoClock.default.play(task, quant)	 }
	*sched { | delta, item | TempoClock.default.sched(delta, item)  }
	*schedAbs { | beat, item | TempoClock.default.schedAbs(beat, item)  }
	*clear { | releaseNodes | TempoClock.default.clear(releaseNodes)	 }
	*tempo_ { | newTempo | TempoClock.default.tempo_(newTempo)  }
	*etempo_ { | newTempo | TempoClock.default.etempo_(newTempo)	 }

	*tempo { ^TempoClock.default.tempo }
	*beats { ^TempoClock.default.beats }
	*beats2secs { | beats | ^TempoClock.default.beats2secs(beats)  }
	*secs2beats { | secs | ^TempoClock.default.secs2beats(secs)	}
	*nextTimeOnGrid { | quant = 1, phase = 0 | ^TempoClock.default.nextTimeOnGrid(quant, phase)	}
	*timeToNextBeat { | quant = 1 | ^TempoClock.default.timeToNextBeat(quant)  }

	*setTempoAtBeat { | newTempo, beats | TempoClock.default.setTempoAtBeat(newTempo, beats)	 }
	*setTempoAtSec { | newTempo, secs | TempoClock.default.setTempoAtSec(newTempo, secs)	 }
	*setMeterAtBeat { | newBeatsPerBar, beats | TempoClock.default.setMeterAtBeat(newBeatsPerBar, beats)	 }

	*beatsPerBar { ^TempoClock.default.beatsPerBar  }
	*baseBarBeat { ^TempoClock.default.baseBarBeat  }
	*baseBar { ^TempoClock.default.baseBar  }
	*playNextBar { | task | ^TempoClock.default.playNextBar(task)  }
	*beatDur { ^TempoClock.default.beatDur  }
	*elapsedBeats { ^TempoClock.default.elapsedBeats	 }
	*beatsPerBar_ { | newBeatsPerBar | TempoClock.default.beatsPerBar_(newBeatsPerBar)  }
	*beats2bars { | beats | ^TempoClock.default.beats2bars(beats)  }
	*bars2beats { | bars | ^TempoClock.default.bars2beats(bars)	}
	*bar { ^TempoClock.default.bar  }
	*nextBar { | beat | ^TempoClock.default.nextBar(beat)  }
	*beatInBar { ^TempoClock.default.beatInBar  }

	archiveAsCompileString { ^true }
}
