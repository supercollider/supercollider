
BeatSched {

	classvar <global;
	
	var clock,tempo;
	var epoch=0.0;
	var nextTask; // for exclusive locks

	var pq,nextAbsTime,nextAbsFunc,nextAbsList;
	
	*new { arg clock,tempo;
		^super.newCopyArgs(clock ? SystemClock,tempo ? Tempo).init
	}		
	*initClass { global = this.new; }
	init {
		pq = PriorityQueue.new;
		nextAbsFunc = nextAbsTime = nil;
	}
	
	*xblock { ^global.xblock }	
	*time { ^global.time }
	*time_ { arg seconds; ^global.time_(seconds) }
	*beat { ^global.beat }
	*beat_ { arg beat; ^global.beat_(beat) }
	*clear { ^global.clear }
	*deltaTillNext { arg quantize;	^global.deltaTillNext(quantize) }
	*tdeltaTillNext { arg quantize; ^global.tdeltaTillNext(quantize) }	
	xblock { nextTask = nil; } // block any subsequent xscheds
	time { ^Main.elapsedTime - epoch }
	time_ { arg seconds; epoch = Main.elapsedTime - seconds; }
	beat { ^tempo.secs2beats(Main.elapsedTime - epoch) }
	beat_ { arg beat;
		epoch = Main.elapsedTime - tempo.beats2secs(beat);
	}
	clear {
		pq.clear;
		nextTask = nextAbsFunc = nextAbsTime = nil;
	}	
	deltaTillNext { arg quantize; // delta beats !
		var beats,next;
		beats = this.beat;
		next = beats.trunc(quantize);
		^if(next == beats,0 ,{ next + quantize - beats }) // now or next
	}
	tdeltaTillNext { arg quantize;
		var beats,next;
		beats = this.beat;
		next = beats.trunc(quantize);
		^if(next == beats,0 ,{ tempo.beats2secs(next + quantize - beats) }) // now or next
	}	
	
	
	/** global scheduler **/
	*tsched { arg seconds,function;
		^global.tsched(seconds,function);
	}
	*xtsched { arg seconds,function;
		^global.xtsched(seconds,function);
	}		
	*sched { arg beats,function;
		^global.sched(beats,function)
	}
	*xsched { arg beats,function;
		^global.xsched(beats,function)
	}
	*qsched { arg quantize,function;
		^global.qsched(quantize,function)
	}
	*xqsched { arg quantize,function;
		^global.xqsched(quantize,function )
	}
	*tschedAbs { arg time,function;
		^global.tschedAbs(time,function)
	}		
	// xtschedAbs
	*schedAbs { arg beat,function;
		^global.tschedAbs(beat,function)
	}

	/**  instance methods **/
	tsched { arg seconds,function;
		clock.sched(seconds,{ arg time; function.value(time); nil })
	}
	xtsched { arg seconds,function;
		var thsTask,notCancelled;
		clock.sched(seconds,
			thsTask = nextTask = {
				if(thsTask === nextTask,function);
				nil
			}
		);
	}
		
	sched { arg beats,function;
		this.tsched(tempo.beats2secs(beats),function)
	}
	xsched { arg beats,function;
		this.xtsched(tempo.beats2secs(beats),function)
	}
	
	qsched { arg quantize,function;
		this.sched(this.deltaTillNext(quantize),function)

	}
	xqsched { arg quantize,function;
		this.xsched(this.deltaTillNext(quantize),function )
	}
	
	tschedAbs { arg time,function;
		if(time >= this.time,{ // in the future ?
			pq.put(time,function);
			// was something else already scheduled before me ?
			if(nextAbsTime.notNil,{
				if(time == pq.topPriority ,{ // i'm next
					pq.put(nextAbsTime,nextAbsList); // put old top back on queue
					this.tschedAbsNext;
				})
			},{
				this.tschedAbsNext; // sched meself
			});
		})			
	}
	// xtschedAbs
	schedAbs { arg beat,function;
		this.tschedAbs(tempo.beats2secs(beat),function)
	}

	// private
	tschedAbsNext  {
		var function,secs;
		var thTask;
		if((nextAbsTime = pq.topPriority).notNil,{
			function = nextAbsList = pq.pop;
			secs = nextAbsTime - this.time;
			clock.sched(secs,thTask = nextAbsFunc = {
				if(thTask === nextAbsFunc,{
					function.value;
					this.tschedAbsNext
				});
				nil
			});
		});
	}
}

OSCSched : BeatSched {
	
	
	/** global scheduler **/
	*tsched { arg seconds,server,message,clientSideFunction;
		^global.tsched(seconds,server,message,clientSideFunction);
	}
	*xtsched { arg seconds,server,message,clientSideFunction;
		^global.xtsched(seconds,server,message,clientSideFunction);
	}		
	*sched { arg beats,server,message,clientSideFunction;
		^global.sched(beats,server,message,clientSideFunction)
	}
	*xsched { arg beats,server,message,clientSideFunction;
		^global.xsched(beats,server,message,clientSideFunction)
	}
	*qsched { arg quantize,server,message,clientSideFunction;
		^global.qsched(quantize,server,message,clientSideFunction)
	}
	*xqsched { arg quantize,server,message,clientSideFunction;
		^global.xqsched(quantize,server,message,clientSideFunction )
	}
	*tschedAbs { arg time,server,message,clientSideFunction;
		^global.tschedAbs(time,server,message,clientSideFunction)
	}		
	// xtschedAbs
	*schedAbs { arg beat,server,message,clientSideFunction;
		^global.tschedAbs(beat,server,message,clientSideFunction)
	}
	
	/**  instance methods **/
	tsched { arg seconds,server,message,clientSideFunction;
		clock.sched(seconds - server.latency,{
			server.sendBundle(server.latency,message);
			nil
		});
		if(clientSideFunction.notNil,{
			clock.sched(seconds,{ clientSideFunction.value; nil })
		});
	}
	xtsched { arg seconds,server,message,clientSideFunction;
		var thTask,notCancelled;
		clock.sched(seconds - server.latency,thTask = nextTask = {
			if(notCancelled = (thTask === nextTask),{
				server.sendBundle(server.latency,message)
			});
			nil
		});
		if(clientSideFunction.notNil,{
			clock.sched(seconds,{ if(notCancelled,clientSideFunction); nil })
		});
	}
		
	sched { arg beats,server,message,clientSideFunction;
		this.tsched(tempo.beats2secs(beats),server,message,clientSideFunction)
	}
	xsched { arg beats,server,message,clientSideFunction;
		this.xtsched(tempo.beats2secs(beats),server,message,clientSideFunction)
	}
	
	qsched { arg quantize,server,message,clientSideFunction;
		this.sched(this.deltaTillNext(quantize),server,message,clientSideFunction)

	}
	xqsched { arg quantize,server,message,clientSideFunction;
		this.xsched(this.deltaTillNext(quantize),server,message,clientSideFunction )
	}
	
	tschedAbs { arg time,server,message,clientSideFunction;
		if(time >= this.time,{ // in the future ?
			pq.put(time,[server,message,clientSideFunction]);
			// was something else already scheduled before me ?
			if(nextAbsTime.notNil,{
				if(time == pq.topPriority ,{ // i'm next
					pq.put(nextAbsTime,nextAbsList); // put old top back on queue
					this.tschedAbsNext;
				})
			},{
				this.tschedAbsNext; // sched meself
			});
		})			
	}
	// xtschedAbs
	schedAbs { arg beat,server,message,clientSideFunction;
		this.tschedAbs(tempo.beats2secs(beat),server,message,clientSideFunction)
	}



	// private
	tschedAbsNext  {
		var list,secs,server;
		var thTask,notCancelled;
		if((nextAbsTime = pq.topPriority).notNil,{
			list = nextAbsList = pq.pop;
			secs = nextAbsTime - this.time;
			server = list.at(0);
			clock.sched(secs - server.latency,thTask = nextAbsFunc = {
				if(notCancelled = (thTask === nextAbsFunc),{
					server.sendBundle(server.latency,list.at(1))
				});
				nil
			});
			clock.sched(secs,{ 
				if(notCancelled,{
					list.at(2).value;
					this.tschedAbsNext;
				}); 
				nil 
			})
		});
	}
}
