
HasSubject : AbstractPlayer {
	
	var <>subject;

	*new { arg subject=0.0;
		^super.new.subject_(subject.loadPath)
	}

	ar { ^subject.ar }
	
	storeParamsOn { arg stream;
		stream.storeArgs([enpath(subject)]);
	}
	children { ^[subject] }
	numChannels { ^subject.tryPerform(\numChannels) ? 1 }
	guiClass { ^HasSubjectGui }
}
/*

Ar : HasSubject { 

	ar {  // same as .value
		if(subject.respondsTo(\ar),{
			^subject.ar
		},{
			^Plug.ar(subject)
		})
	}
	
	next { ^this.value }
	
	doesNotUnderstand { arg selector ... args;
		^subject.performList(selector,args)
	}
	// dodgy
	canDoSpec { arg aspec; ^(aspec.isKindOf(AudioSpec) or: {aspec.canKr}) } // could go into any kr too

}


AsStream : HasSubject {  // this is for passing in patterns or streams to Patches

	ar {
		^subject.asStream
	}
	
	next { ^this.value }

	doesNotUnderstand { arg selector ... args;
		^subject.performList(selector,args)
	}

}

*/

/*

Kr : Ar { // should all be moved under KrPlayer

	var <>lag;
	
	*new { arg subject=0.0,lag=0.0;
		^super.new(subject).lag_(lag)
	}
	value { ^this.kr }
	ar { ^K2A.ar(this.kr) } 
	kr {
		if(subject.respondsTo(\kr),{
			^subject.kr(lag.value)
		},{
			^Plug.kr(subject.asStream,lag.value)  //  'subject' could be a number, function or .value responding object
		})
	}
	spec_ { arg aspec; // respondsTo
		subject.tryPerform('spec_',aspec);
	}
	value_ { arg val;
		subject.tryPerform('value_',val);
	}
	canDoSpec {arg aspec; ^aspec.canKr }
	storeParamsOn { arg stream;
		stream.storeArgs([ enpath( subject ),lag ]);	
	}
	children { ^[subject,lag] }

	guiClass { ^KrGui }
	
}

*/



/*

StreamKr : HasSubjectKr { // trigger is a kr rate trigger
	
	var <>trigger;
	
	*new { arg subject=0.0,trigger=0.0;
		^super.new(subject).trigger_(trigger)
	}


	
//	kr { 
//		var k;
//		k=Sequencer.kr(subject.asStream,trigger.value);
//	}
	
	storeParamsOn { arg stream;
		stream.storeArgs([enpath(subject),trigger]);
	}
	children { ^[subject,trigger] }
	guiClass { ^StreamKrGui }
	
}

*/




StreamKrDur : HasSubject { // Synthless, above player

	var <>durations;
	var tempo,routine,nodeControl,firstValue;
	
	*new { arg values=0.0,durations=0.0;
		// check if can become a BufSeqDur
		// make refs of arrays into Pseq
		^super.new(values).durations_(durations.loadPath).skdinit
	}
	skdinit {
		tempo = Tempo.default;
		firstValue = subject.asStream.next;
		/* for musical accuracy, need to run ahead and use OSCSched
		to deliver */
		routine = Routine({
					var dur,val;
					var valst,durst;
					valst = subject.asStream;
					durst = durations.asStream;
					while({
						(val = valst.next).notNil and: {
							(dur = durst.next).notNil;
						}
					},{
						// send the message...	
						nodeControl.value_(val);
						tempo.beats2secs(dur).wait
					});
				});
	}
	// has no synth, just a bus
	loadDefFileToBundle {}
	rate { ^\control }
	children { ^[] } // synthChildren synthArgObjects inputs
	spawnAtTime { arg atTime;
		// depending on didSpawn to start for now
	}
	spawnToBundle {
		this.spawnAtTime;
	}
	didSpawn {	arg patchIn,synthi;
		patchOut.connectTo(patchIn,false);
		nodeControl = patchIn.nodeControl;
		SystemClock.play(routine)
	}
	instrArgRate { ^\control }
	instrArgFromControl { arg control;
		^control
	}
	synthArg {
		^firstValue
	}

	free {
		routine.stop;
		routine.reset;
	}
	// shouldn't happen
	synthDefArgs {}
	asSynthDef {}
	
	
	
	/*
	// for BufSeqDur
	prepareForPlay { arg group,bundle;
		var values,durs,data;
		super.prepareForPlay(group,bundle);
		values = subject.oneCycle;
		durs = durations.oneCycle;
		minTime = durs.minItem;
		data = Array.new;
		durs.do({ arg d,i;
			
		})
			
		buffer = subject.asBufferToBundle(group,bundle);
		
		tempoBus = TempoBus.new;
		tempoBus.prepareForPlay(group,bundle);
	}

	synthDefArgs { ^[buffer.synthArg,tempoBus.synthArg,framesPerBeat,patchOut.synthArg] }
	asSynthDef {
		^SynthDef(this.defName,{ arg i_bufnum=0, i_tempoBusIndex=0,i_framesPerBeat=4.0,out=0;
			var phasor,tempo,numFrames,value,cpb;
			
			tempo = In.kr(i_tempoBusIndex) * i_framesPerBeat;

			numFrames = BufFrames.kr(i_bufnum);
			cpb = i_framesPerBeat / numFrames;
			phasor = LFSaw.kr(tempo / cpb).range(0,numFrames);
			value = Index.kr(i_bufnum, phasor);
			Out.kr(out,
				value
			)
		})
	}
	
	
	kr { 
		var k,t;
		k=Sequencer.kr(subject.asStream,StepClock.kr(trigger.asStream,GetTempo.kr));
		^if(lag != 0.0,{
			Lag.kr(k,lag.value)
		},{
			k
		})
	}
	
	*/
}



/*
Stream2Trig : StreamKrDur { // outputs just a single pulse trig
	// has no lag
	kr { 
		^ImpulseSequencer.kr(subject.asStream,StepClock.kr(trigger.asStream,GetTempo.kr));
	}
}
*/




/**

StreamKrDriver( value stream,duration stream)
	drive a plug:
	xline
	line
	
**/

//ArPause : Ar {
//	
//	var tog;
//	
//	gui { arg lay;
//		tog=ToggleButton(layout,subject.name,{ subject.resume4 },{ subject.pause });
//	}
//	
//	value {
//		tog.passiveToggle(true);
//		^subject.ar
//	}
//
//}




