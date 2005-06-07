//	Splay and SplayZ updated and moved to common. adc 

//Splay { // spreads each item out evenly across stereo pan
//
//	*arFill { arg n,function;
//		var v;
//		v = n.reciprocal * 2;
//		^Mix.arFill(n.asInteger,{arg i;
//			LinPan2.ar(
//				function.value(i), 	
//				//( i * (2/n) - (n - 1 * (2/n)  / 2) )
//				(2 * i - n + 1) / n // math by jr
//			);
//		}) * v
//	}
//	
//	*ar { arg ugenArray;
//		var n,v;
//		n=ugenArray.size;
//		v = n.reciprocal;
//		^Mix.arFill(n,{ arg i;
//			LinPan2.ar(
//				ugenArray.at(i), 
//				(2 * i - n + 1) / n
//			);
//		}) * v
//	}
//}
//
//
//// by Jonathan Segel <jsegel@magneticmotorworks.com>
//// spreads each item out evenly across multi-channel pan
// 
//SplayZ : Splay {
//
//	*arFill { arg n, c=4, function;
//		var v;
//		v = n.reciprocal * 2;
//		^Mix.arFill(n.asInteger,{arg i;
//					PanAz.ar(c,
//						function.value(i),
//						(2 * i - n + 1) / n
//					);
//		}) * v
//	}
//	
//	*ar { arg c=4, ugenArray;
//		var n,v;
//		n=ugenArray.size;
//		v = n.reciprocal;
//		^Mix.arFill(n,{ arg i;
//			PanAz.ar(c,
//				ugenArray.at(i),
//				(2 * i - n + 1) / n
//			);
//		}) * v
//	}
//}
//// adding a spread ratio and center would be cool - cx
//
//
//

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
		whiches = PanAz.ar(inputs.size,1.0,bipolar,width);

		^Mix.ar(
			inputs.collect({ arg sound,i;
				sound * whiches.at(i)
			})
		)
	}
	*kr { arg inputs, bipolar,width=2.0;
		var whiches;
		inputs = inputs.dereference;
		whiches = PanAz.ar(inputs.size,1.0,bipolar,width);

		^Mix.ar(
			inputs.collect({ arg sound,i;
				sound * whiches.at(i)
			})
		)
	}
}


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
		^audio.value * EnvGen.kr(env,gate,gated)
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
