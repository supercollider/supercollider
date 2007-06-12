
BusDriver : SynthlessPlayer {

	var <>lag=0.0,<>latency=0.2;
	var sched,bus,msg,bnd,resched,routine;
	
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
	stopToBundle { arg bundle;
		super.stopToBundle(bundle);
		bundle.addMessage(this,\reset);
	}
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
		bundle.addMessage(this,\didSpawn);
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

