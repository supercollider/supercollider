
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

Kr : Ar { // should all be moved under ControlPlayer

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

	*guiClass { ^KrGui }
	
}

*/

// Sequencer

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
	*guiClass { ^StreamKrGui }
	
}

*/


/*

//Stream2Kr
StreamKrDur : StreamKr { // trigger is a stream of beat durations

	kr { 
		var k,t;
		k=Sequencer.kr(subject.asStream,StepClock.kr(trigger.asStream,GetTempo.kr));
		^if(lag != 0.0,{
			Lag.kr(k,lag.value)
		},{
			k
		})
	}
}

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




