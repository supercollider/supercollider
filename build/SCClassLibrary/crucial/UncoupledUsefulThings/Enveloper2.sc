

// audio function plays continously but is gated by the envelope
Enveloper2 { 
		
	*ar { arg audio,gate,env;
		var ts;
		env ?? {env = Env.adsr};
		^audio * EnvGen.kr(env,gate,Latch.kr(gate,gate))
	}

}





/* backwards */



// NB a nil * error means your audio function failed
// i wont put error checking in here though
/*
ReTrigger {//to be deprec ?

	*ar { arg func,trig=0.0,dur=0.3;
	
		var env,ts;
		env=Env.perc(0.0,dur);
		
		ts=TSpawn.ar({arg spawn; func.value * EnvGen.kr(env,trig.poll)   },1,trig: trig);
		// tspawn fails to trigger if the first cycle is already non zero
		thisSynth.sched(0.0,{ if(trig.poll > 0.0,{ts.source.trigger })});
		^ts
	}

}

ReTrigger2 { 
		// input re.valued each time
		
	*ar { arg audio,trig,env,maxVoices,numChannels=1;
		var ts,amp;

		env ?? {env = Env.adsr};
		amp = Latch.kr(trig,trig);
		ts=TSpawn.ar({arg spawn,i,synth; 
			spawn.releaseAll;
			(audio.value * EnvGen.kr(env,amp) )
		},numChannels,nil,trig);
		// tspawn fails to trigger if the first cycle is already non zero
		thisSynth.sched(0.0,{ if(trig.poll > 0.0,{ts.source.trigger })});
		if(maxVoices.notNil,{ ts.source.maxVoices = maxVoices });
		^ts
	}
}

*/

Impulsar {  // see also Trig
	// this is just one sample impulse at .ar triggered by a .kr trig
	// K2A.ar(trig)  sounds different, being a whole control period
	// and has K2A interpolatiion

	*ar { arg trig=0.0,mul=1.0,add=0.0;
		var t;
		t=Trig.ar(trig,2.26757e-05);
		^t.madd(mul,add)
		
		// was this in sc2, sounds different
		//^ImpulseSequencer.ar({trig.poll},K2A.ar(trig),mul,add)
		
	}	
}

// these supercede ReTrigger
// audio can be a function or an already playing sound

// audio function re-valued each spawn event
/*
Enveloper {  
	// fixed dur, trigged by trig
			
	*ar { arg audio,trig,env,maxVoices,numChannels=1;
		var ts;
		env ?? { env = Env.linen(0.01,0.4,0.3) };

		ts=TSpawn.ar({arg spawn; audio.value * EnvGen.kr(env,trig.poll)   },numChannels,trig: trig);
		// tspawn fails to trigger if the first cycle is already non zero
		thisSynth.sched(0.0,{ if(trig.poll > 0.0,{ts.source.trigger })});
		if(maxVoices.notNil,{ ts.source.maxVoices = maxVoices });
		^ts
	}
}

*/


