

BusDriver : SynthlessPlayer {

	var <>lag=0.0,<>latency=0.2;
	var sched,bus,msg,bnd,resched,routine;
	
	loadDefFileToBundle {}
	children { ^[] } // inputs
	rate { ^\control }
	instrArgFromControl { arg control;
		^if(lag == 0.0,{
			In.kr(control,1)
		},{
			Lag.kr(In.kr(control,1),lag)
		})
	}
	synthArg {
		^patchOut.synthArg
	}

	reset {
		sched.clear;
	}
	stop { 
		this.reset;
		super.stop;
	}
	stopToBundle { arg b;
		super.stopToBundle(b);
		b.addAction(this,\reset);
	}
	free {
		this.reset;
	}
	
	// shouldn't happen
	synthDefArgs {}
	asSynthDef {}
}

StreamKrDur : BusDriver {

	var <>values,<>durations;
	var valst,durst;

	*new { arg values=0.0,deltas=0.25,lag=0.0;
		// make refs of arrays into Pseq
		if(values.isKindOf(Ref),{
			values = Pseq(values.value,inf);
		});
		if(deltas.isKindOf(Ref),{
			deltas = Pseq(deltas.value,inf);
		});
		^super.new.values_(values).durations_(deltas.loadDocument).lag_(lag).skdinit
	}
	skdinit {
		sched = TempoClock.default;
		routine = Routine({
			var dur,val,server;
			server = this.server;
			latency = server.latency;
			//first val already sent
			dur = durst.next;
			// small slippage if tempo changes during first event.
			// but only within the window of latency
			(dur - Tempo.secs2beats(latency)).yield;
			while({
				(val = valst.next).notNil and:
				(dur = durst.next).notNil
			},{
				msg.put(2,val);
				server.listSendBundle(latency,bnd);
				dur.yield
			});
		});
		valst = values.asStream;
		durst = durations.asStream;
	}
	reset {
		routine.stop;
		// should be able to just reset these
		valst = values.asStream;
		durst = durations.asStream;
	}		
	// has no synth, just a bus
	spawnToBundle { arg bundle;
		bus = patchOut.bus;
		msg = ["/c_set",bus.index,valst.next];
		bnd = [msg];
		// send first message
		bundle.add(msg);
		bundle.addAction(this,\didSpawn);
	}
	didSpawn {
		routine.reset;
		//valst = values.asStream;
		//durst = durations.asStream;
		sched.schedAbs(sched.elapsedBeats, routine);
		//sched.play(routine);
		super.didSpawn;
	}
	storeArgs { ^[values,durations,lag] }
	guiClass { ^StreamKrDurGui }
	
}


Stream2Trig : StreamKrDur { // outputs just a single pulse trig

	// doesn't use lag	
	instrArgFromControl { arg control;
		^InTrig.kr( control,1)
	}
}

/**
XOX : BusDriver	{ // eg. roland 707 808 909 and 727 for the latino market
	var <list;
	var <stepsize=0.25,  next = 0,<position=0;
	var <deltaTable,nextTable,last;

	*new	{	arg thing=16,stepsize=0.25;
		^super.new.initxox(thing,stepsize)
	}
	
	initxox {arg thing,st;
		if(thing.isNumber,{ list = Array.fill(thing,{ [0,1.0.rand].choose }); });
		last = list.size - 1;
		if(st.notNil,{ stepsize=st; });
		nextTable=Array.newClear(list.size);
		deltaTable=nextTable.copy;
		this.initTables;

		sched = OSCSched.new;
		resched = {
			position = next;
			this.changed(\position,position);
			next = nextTable.at(position);
			msg.put(2,list.at(position));		
			sched.xsched(stepsize * deltaTable.at(position),this.server,bnd,resched);
		};
	}

	spawnToBundle { arg bundle;
		bus = patchOut.bus;
		msg = ["/c_set",bus.index,list.at(0)];
		bnd = [msg];
		// send first message
		bundle.add(msg);
		bundle.addFunction(resched);
	}
	
	instrArgFromControl { arg control;
		^InTrig.kr( control,1)
	}

	//position { ^next }

	initTables {  
		var mynext, delta;
		list.do({arg l,i;
		
			mynext=this.findNextAfter(i);
			
			if(mynext.notNil,{
				delta = (((mynext-i).wrap(0,last) * stepsize ));
				if(delta<=0,{delta=stepsize * list.size});
					// one bar long 
				
			},{	// nothing playing at all
				// one bar long for now
				mynext=i;
				delta=stepsize * list.size;
			});
			deltaTable.put(i,delta);
			nextTable.put(i,mynext);
		});
	}

	findNextAfter {arg afterme; 
		var index;
		list.size.do({arg i;
			index=(i + afterme + 1).wrap(0,last);
			if((list@index).isStrictlyPositive,{^index})
		})
		^nil
	}
	 
	put { arg i,val;
		list.put(i,val);
		this.initTables;
		this.changed;	
	}
		
	rand {	
		if(list.isNil,{
			list=Array.fill(list.size,{ spec.map(1.0.rand)})
		});
		list=list.scramble; this.initTables;this.changed;
	}
		
	fill { arg density;
		list=Array.fill(list.size,{arg i; if(i<density,1,0)});
		this.rand;
		this.changed;
	}
	
	rotate { arg way;
		list=list.rotate(way);
		this.initTables;
		this.changed;
	}
	
	stepsize_ { arg st; stepsize=st; this.initTables }
	storeArgs { ^[list,stepsize] }	
	guiClass { ^XOXGui }
}

**/

