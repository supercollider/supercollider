
BeatSched {

	classvar <global;
	
	var clock,tempo,tempoClock;
	var epoch=0.0;
	var nextTask; // for exclusive locks

	var pq,nextAbsTime,nextAbsFunc,nextAbsList;
	
	*new { arg clock,tempo,tempoClock;
		^super.newCopyArgs(clock ? SystemClock,
			tempo ?? {Tempo.default},
			tempoClock ?? {TempoClock.default}).init
	}		
	*initClass { 
		Class.initClassTree(Tempo);
		global = this.new; 
	}
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
	deltaTillNext { arg quantize; // delta in beats till next beat
		var beats,next;
		beats = this.beat;
		next = beats.trunc(quantize);
		^if(next == beats,0 ,{ next + quantize - beats }) // now or next
	}
	tdeltaTillNext { arg quantize; // delta in seconds till next beat
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
	*xtschedAbs { arg time,function;
		^global.xtschedAbs(time,function)
	}
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
		tempoClock.sched(beats,function)
		//this.tsched(tempo.beats2secs(beats),function)
	}
	xsched { arg beats,function;
		var thsTask,notCancelled;
		tempoClock.sched(beats,
			thsTask = nextTask = {
				if(thsTask === nextTask,function);
				nil
			}
		);
		//this.xtsched(tempo.beats2secs(beats),function)
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
	// will cancel all pending
	xtschedAbs { arg time,function;
		if(time >= this.time,{ // in the future ?
			pq.clear;
			this.xblock;
			this.tsched(time,function)
		});
	}
	schedAbs { arg beat,function;
		tempo.schedAbs(beat,function);
		//this.tschedAbs(tempo.beats2secs(beat),function)
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
	classvar <global;
	
	*initClass { global = this.new; }

	/** global scheduler **/
	*tsched { arg seconds,server,bundle,clientSideFunction;
		^global.tsched(seconds,server,bundle,clientSideFunction);
	}
	*xtsched { arg seconds,server,bundle,clientSideFunction;
		^global.xtsched(seconds,server,bundle,clientSideFunction);
	}		
	*sched { arg beats,server,bundle,clientSideFunction;
		^global.sched(beats,server,bundle,clientSideFunction)
	}
	*xsched { arg beats,server,bundle,clientSideFunction;
		^global.xsched(beats,server,bundle,clientSideFunction)
	}
	*qsched { arg quantize,server,bundle,clientSideFunction;
		^global.qsched(quantize,server,bundle,clientSideFunction)
	}
	*xqsched { arg quantize,server,bundle,clientSideFunction;
		^global.xqsched(quantize,server,bundle,clientSideFunction )
	}
	*tschedAbs { arg time,server,bundle,clientSideFunction;
		^global.tschedAbs(time,server,bundle,clientSideFunction)
	}		
	// xtschedAbs
	*schedAbs { arg beat,server,bundle,clientSideFunction;
		^global.tschedAbs(beat,server,bundle,clientSideFunction)
	}
	
	/**  instance methods **/
	tsched { arg seconds,server,bundle,clientSideFunction;
		clock.sched(seconds - server.latency,{
			server.listSendBundle(server.latency,bundle);
			nil
		});
		if(clientSideFunction.notNil,{
			clock.sched(seconds,{ clientSideFunction.value; nil })
		});
	}
	xtsched { arg seconds,server,bundle,clientSideFunction;
		var thTask,notCancelled;
		clock.sched(seconds - server.latency,thTask = nextTask = {
			if(notCancelled = (thTask === nextTask),{
				server.listSendBundle(server.latency,bundle)
			});
			nil
		});
		if(clientSideFunction.notNil,{
			clock.sched(seconds,{ if(notCancelled,clientSideFunction); nil })
		});
	}
		
	sched { arg beats,server,bundle,clientSideFunction;
		tempoClock.sched(beats - server.latency,{ // lazily using the seconds as beats
			// inaccurate final delivery if tempo is changing quickly
			server.listSendBundle(tempo.beats2secs(server.latency),bundle);
			nil
		});
		if(clientSideFunction.notNil,{
			tempoClock.sched(beats,{ clientSideFunction.value; nil })
		});
	}
	xsched { arg beats,server,bundle,clientSideFunction;
		var thTask,notCancelled;
		tempoClock.sched(beats - server.latency,thTask = nextTask = {
			if(notCancelled = (thTask === nextTask),{
				server.listSendBundle(tempo.beats2secs(server.latency),bundle)
			});
			nil
		});
		if(clientSideFunction.notNil,{
			tempoClock.sched(beats,{ if(notCancelled,clientSideFunction); nil })
		});
	}
	
	qsched { arg quantize,server,bundle,clientSideFunction;
		this.sched(this.deltaTillNext(quantize),server,bundle,clientSideFunction)

	}
	xqsched { arg quantize,server,bundle,clientSideFunction;
		this.xsched(this.deltaTillNext(quantize),server,bundle,clientSideFunction )
	}
	
	tschedAbs { arg time,server,bundle,clientSideFunction;
		if(time >= this.time,{ // in the future ?
			pq.put(time,[server,bundle,clientSideFunction]);
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
	schedAbs { arg beat,server,bundle,clientSideFunction;
		/*  to do...
		if(time >= this.time,{ // in the future ?
			pq.put(time,[server,bundle,clientSideFunction]);
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
		*/
		this.tschedAbs(tempo.beats2secs(beat),server,bundle,clientSideFunction)
	}

	xschedCXBundle { arg beatDelta,server,cxbundle;
		this.xsched(beatDelta,server,cxbundle.messages,{cxbundle.doFunctions});
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
					server.listSendBundle(server.latency,list.at(1))
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
