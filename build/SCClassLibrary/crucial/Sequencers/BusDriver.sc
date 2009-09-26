
AbstractBusDriver : SynthlessPlayer {

	var <>lag=0.0,<>latency=0.1;
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
		bundle.addMessage(this,\didStop);
	}
}

BusDriver : AbstractBusDriver {

	var <value,<>spec;

	*new { |value=0.0,spec=\unipolar|
		^super.new.spec_(spec.asSpec).value_(value)
	}

	poll { ^value }
	value_ { |v|
		value = spec.constrain(v);
		if(this.isPlaying,{
			this.bus.set(value)
		});
	}
	activeValue_ { |v|
		this.value = v;
		this.changed;
	}
	spawnToBundle { |b|
		var v;
		super.spawnToBundle(b);
		b.add(this.bus.setMsg(value));
		v = value;
		b.addFunction({// just in case it changed
			if(v != value,{
				this.bus.set(value);
			})
		});
	}
	storeOn { |stream| value.storeOn(stream) }
	guiClass { ^KrNumberEditorGui }
	storeArgs { ^[value,spec] }
}



/*
StreamKr : Kr { // trigger is a kr rate trigger

	var <>trigger;

	*new { arg subject=0.0,trigger=0.0,lag=0.0;
		^super.new(subject,lag).trigger_(trigger)
	}

	kr {
		var k;
		k=Sequencer.kr(subject.asStream,trigger.value);
		^if(lag != 0.0,{
			Lag.kr(k,lag.value)
		},{
			k
		})
	}

	storeParamsOn { arg stream;
		stream.storeArgs([this.enpath(subject),trigger,lag]);
	}
	children { ^[subject,trigger,lag] }
	*guiClass { ^StreamKrGui }

}
*/

StreamKrDur : AbstractBusDriver {

	var <>values,<>durations;
	var valst,deltast,<lastValue,<delta,<beat;

	*new { arg values=0.0,durations=0.25,lag=0.0;
		// make refs of arrays into Pseq
		if(values.isKindOf(Ref),{
			values = Pseq(values.value,inf);
		});
		if(durations.isKindOf(Ref),{
			durations = Pseq(durations.value,inf);
		},{
			durations = durations.loadDocument;
		});
		^super.new.values_(values).durations_(durations).lag_(lag).skdinit
	}
	skdinit {
		// if values is of spec EventStreamSpec, throw an error (Pbind)

		sched = TempoClock.default;
		routine = Routine({
			var val,server,beatsTillNext,beatLatency;
			server = this.server;
			latency = server.latency ? 0.05;
			beatLatency = Tempo.secs2beats(latency);

			// first val already sent
			delta = deltast.next ?? {routine.stop};
			beat = beat + delta;
			// what beat am I really supposed to play at ?
			// this is accurate enough for now.  its just minor scheduler drift

			beatsTillNext = delta - beatLatency;
			// this goes negative if delta is less than server latency
			while({ beatsTillNext < beatLatency},{
				// so make up some time
				// beatsTillNext.debug("beatsTillNext");

				// send immediately
				val = valst.next ?? {routine.stop};
				lastValue = val;
				msg.put(2,val);
				server.listSendBundle( Tempo.beats2secs(delta), bnd);

				delta = delta + deltast.next ?? {routine.stop};

				beatsTillNext = delta - beatLatency;
			});
			//beatsTillNext.yield;
			beatLatency.yield;

			// would be better to calc the exact time based on scheduling slippage
			while({
				(delta = deltast.next(this)).notNil and:
				(val = valst.next(this)).notNil
			},{
				msg.put(2,val);
				server.listSendBundle(latency,bnd);
				lastValue = val;
				delta.yield
			});
		});
	}
	didStop { this.stopRoutine }
	stopRoutine {
		routine.stop;
	}
	resetRoutine {
		routine.reset;
	}
	// has no synth, just a bus
	spawnToBundle { arg bundle;
		valst = values.asStream;
		deltast = durations.asStream;
		beat = 0.0;
		bus = patchOut.bus;
		lastValue = valst.next;
		if(lastValue.notNil,{
			msg = ["/c_set",bus.index,lastValue];
			bnd = [msg];
			// send first message
			bundle.add(msg);
			bundle.addMessage(this,\didSpawn);
		});
	}
	didSpawn {
		routine.reset;
		// immediately
		sched.schedAbs(sched.elapsedBeats, routine);
		//because sched.play(routine); <- this uses nexttime on grid

		super.didSpawn;
	}
	poll { ^lastValue }
	spec {
		var valuesSpec,makeSpec;
		// values might have a stream spec, who has an item spec that is the result
		valuesSpec = values.tryPerform(\spec);
		if(valuesSpec.notNil,{
			if(valuesSpec.isKindOf(StreamSpec),{
				valuesSpec = valuesSpec.itemSpec;
				if(valuesSpec.isKindOf(ControlSpec),{
					makeSpec = ControlSpec(valuesSpec.minval,valuesSpec.maxval,valuesSpec.warp,
							valuesSpec.step,valuesSpec.default,valuesSpec.units);
					^makeSpec
				});
			})
		});
		if(values.isNumber,{
			^ControlSpec(values,values,default: values)
		});
		// else I couldn't tell you so we continue to guess "control"
		^super.spec
	}

	children { ^[values,durations] }
	storeArgs { ^[values,durations,lag] }
	guiClass { ^StreamKrDurGui }
}


Stream2Trig : StreamKrDur { // outputs just a single pulse trig with the amplitude of the value stream

	// *new(amplitudes, deltas)

	// doesn't use lag
	instrArgFromControl { arg control;
		^InTrig.kr( control,1)
	}
	poll { ^nil }
	spec { ^\trig.asSpec }
}

