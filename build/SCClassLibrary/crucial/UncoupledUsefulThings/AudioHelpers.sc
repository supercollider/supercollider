
NumChannels {

	*ar { arg input,numChannels=2,mixdown=true;

		if(input.size > 1,{// collection
		   ^input
			.clump(input.size / numChannels)
			.collect({arg chan,i;
				if(chan.size == 1,{
					chan.at(0)
				},{
					if(mixdown,{
						Mix.ar(chan)
					},{
						chan.at(0)
					})
				})
			})
		},{ // single ugen or single item collection
			if(input.isSequenceableCollection,{
				input = input.at(0);
			});

			if(numChannels == 1,{
				^input
			},{
				^Array.fill(numChannels,input)
			})
		})
	}

}

XFader  {  // UNIPOLAR
		// XFade2 is now cheaper (in c)

	*ar { arg l, r, pan;
		^(l + (pan * (r - l)))
	}
	*kr { arg l, r, pan;
		^(l + (pan * (r - l)))
	}
}


XFader4  {

	// quad stereo cross fade
	*ar { arg lf, rf, lb, rb, xpan, ypan; // unipolar
		var front, back;
		front = lf + (xpan * (rf - lf));
		back  = lb + (xpan * (rb - lb));
		^back + (ypan * (front - back))
	}
	*kr { arg lf, rf, lb, rb, xpan, ypan;
		var front, back;
		front = lf + (xpan * (rf - lf));
		back  = lb + (xpan * (rb - lb));
		^back + (ypan * (front - back))
	}
}


XFaderN  {

	*ar { arg inputs, bipolar,width=2.0;
		var whiches;
		inputs = inputs.dereference;
		whiches = PanAz.ar(inputs.size,SinOsc.ar(0.0,add:1.0),bipolar,width);

		^Mix.ar(
			inputs.collect({ arg sound,i;
				sound * whiches.at(i)
			})
		)
	}
	*kr { arg inputs, bipolar,width=2.0;
		var whiches;
		inputs = inputs.dereference;
		whiches = PanAz.ar(inputs.size,SinOsc.ar(0.0,add:1.0),bipolar,width);

		^Mix.ar(
			inputs.collect({ arg sound,i;
				sound * whiches.at(i)
			})
		)
	}
}



Mono {
	*new { arg input;
		if(input.isArray,{
			^input.at(0)
		},{
			^input
		})
	}
}


// audio function plays continously but is gated by the envelope
Enveloper2 {

	*ar { arg audio,gate,env;
		var ts,gated;
		gated = Latch.kr(gate,gate);
		env ?? {env = Env.adsr};
		// backwards compatibility : value the function
		if(audio.class === Function,{ audio = audio.value });
		^audio * EnvGen.kr(env,gate,gated)
	}

}


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
