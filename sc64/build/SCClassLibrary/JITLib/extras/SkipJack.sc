SkipJack { 

	classvar <>verbose = true, <all, <defaultClock;
	var updateFunc, <>dt, <>stopTest, <name, <clock, <task, restartFunc; 
	
	*new { arg updateFunc, dt = 0.2, stopTest = false, name = "anon", clock;
		^super.newCopyArgs(updateFunc, dt, stopTest, name, clock).init.start;
	}
	
	*initClass { 
		this.stopAll; 
		all = IdentitySet[]; 
		defaultClock = AppClock;
	}
	
	*stop { |name| all.do { |skip| if (skip.name == name) { skip.stop } }; }
	
	*stopAll { all.do(_.stop).clear; } 
	
	init {
		task = Task ({
			if( verbose )	{ ("SkipJack" + name + "starts.").postln }; 
				while { dt.wait; stopTest.value.not } { updateFunc.value };
				this.stop;
			}, clock ? defaultClock);
	}

	cmdPeriod {
		task.stop.play;
		if( verbose ) { ("SkipJack" + name + "is back up.").postcln };
	}
		
	start { 
		task.stop.play;
		all.add(this);
		CmdPeriod.add(this);
	}
	
	stop { 
		task.stop;
		all.remove(this);
		CmdPeriod.remove(this);
		if( verbose ) { ("SkipJack" + name + "stopped.").postcln }; 
	}
}

Watcher : SkipJack { 
	*new { arg name = "anon", updateFunc, dt=0.2, stopTest = false;
		"Watcher is only for backward compatibility, use SkipJack!".postln;
		^super.newCopyArgs(updateFunc, dt, stopTest, name).init.start;
	}
}	
