// clocks for timing threads.

Clock {
	// abstract class
	*play { arg task;
		this.sched(task.value(thisThread.time), task)
	}
}

SystemClock : Clock {
	*clear {
		_SystemClock_Clear
		^this.primitiveFailed
	}
	*sched { arg delta, item;
		_SystemClock_Sched
		^this.primitiveFailed
	}
	*schedAbs { arg time, item;
		_SystemClock_SchedAbs
		^this.primitiveFailed
	}
}

AppClock : Clock {
	classvar scheduler;
	*initClass {
		scheduler = Scheduler.new(true);
	}
	*sched { arg delta, item;
		scheduler.sched(delta, item)
	}
	*tick {
		scheduler.time = Main.elapsedTime;
	}
	*clear {
		scheduler.clear;
	}
}


Scheduler {
	var drift, time = 0.0, queue;
	
	*new { arg drift = false;
		^super.newCopyArgs(drift).init;
	}
	init {
		time = thisThread.time;
		queue = PriorityQueue.new;
	}

	play { arg task;
		this.sched(task.value(thisThread.time), task)
	}

	sched { arg delta, item;
		var fromTime;
		if (delta.notNil, { 
			fromTime = if (drift, { Main.elapsedTime },{ time });
			queue.put(fromTime + delta, item);
		});
	}
	clear { queue.clear }
	isEmpty { ^queue.isEmpty }
	
	advance { arg delta;
		this.time = time + delta;
	}
	
	time_ { arg newTime;
		var delta, item;
		while ({ 
			time = queue.topPriority; 
			time.notNil and: { time <= newTime }
		},{ 
			item = queue.pop;
			delta = item.awake( time );
			this.sched(delta, item); 
		});
		time = newTime;
	}
}

