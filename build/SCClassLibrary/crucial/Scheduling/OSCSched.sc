
OSCSched {
	
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
	*xblock { ^global.xblock }	
	*time { ^global.time }
	*beat { ^global.beat }
	*time_ { arg seconds; ^global.time_(seconds) }
	*beat_ { arg beat; ^global.beat_(beat) }
	*clear { ^global.clear }
	
	/**  instance methods **/
	tsched { arg seconds,server,message,clientSideFunction;
		clock.sched(seconds - server.delay,{
			server.sendBundle(server.delay,message);
			nil
		});
		if(clientSideFunction.notNil,{
			clock.sched(seconds,{ clientSideFunction.value; nil })
		});
	}
	xtsched { arg seconds,server,message,clientSideFunction;
		var thisTask,notCancelled;
		clock.sched(seconds - server.delay,thisTask = nextTask = {
			if(notCancelled = (thisTask === nextTask),{
				server.sendBundle(server.delay,message)
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
				if(nextAbsTime != pq.topPriority ,{ // i'm next
					pq.put(nextAbsTime,nextAbsList); // put back on queue for later
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

	xblock { nextTask = nil; } // block any subsequent xscheds
	
	time { ^Main.elapsedTime - epoch }
	time_ { arg seconds; epoch = seconds; }
	beat { ^tempo.secs2beats(Main.elapsedTime - epoch) }
	beat_ { arg beat;
		epoch = Main.elapsedTime - tempo.beats2secs(beat);
	}
	clear {
		pq.clear;
		nextTask = nextAbsFunc = nextAbsTime = nil;
	}

	
	// private
	deltaTillNext { arg quantize;
		var beats,next;
		beats = this.beat;
		next = beats.trunc(quantize);
		^if(next == beats,0 ,{ next + quantize - beats }) // now or next
	}	

	
	tschedAbsNext  {
		var list,secs,server;
		var thisTask,notCancelled;
		if((nextAbsTime = pq.topPriority).notNil,{
			list = nextAbsList = pq.pop;
			secs = nextAbsTime - this.time;
			server = list.at(0);
			clock.sched(secs - server.delay,thisTask = nextAbsFunc = {
				if(notCancelled = (thisTask === nextAbsFunc),{
					server.sendBundle(server.delay,list.at(1))
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
