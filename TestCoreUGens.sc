/*
s.boot;
TestCoreUGens.run
*/
TestCoreUGens : UnitTest {
test_ugen_generator_equivalences {
	var n, v;
	this.bootServer;
	
	// These pairs should generate the same shapes, so subtracting should give zero.
	// Of course there's some rounding error due to floating-point accuracy.
	Dictionary[
	 //////////////////////////////////////////
	 // Ramp generators:
	 "Line.ar can match LFSaw.ar" -> {Line.ar(0,1,1) - LFSaw.ar(0.5)},
	 "Line.kr can match LFSaw.kr" -> {Line.kr(0,1,1) - LFSaw.kr(0.5)},
	 "Line can match crossfaded DC" -> {Line.ar(0,1,1) - LinXFade2.ar(DC.ar(0), DC.ar(1), Line.ar(-1,1,1))},
	 // (Integrator goes a bit off ramp cos of roundoff error accumulations)
	 "Line.ar can match integrated DC" -> {Line.ar(0,1,1) - Integrator.ar(DC.ar(SampleDur.ir))},
	 "Line.ar can match EnvGen.ar with slope Env" -> {Line.ar - EnvGen.ar(Env([0,1],[1]))},

	 //////////////////////////////////////////
	 // Triggers:
	 "Trig.ar(_,0) is no-op when applied to Impulse.ar, whatever the amplitude of the impulses" 
	 		-> {n = Impulse.ar(400)*SinOsc.ar(1).range(0,1); Trig.ar(n,0) - n},
	 "Trig1.ar(_,0) has same effect as (_>0) on variable-amplitude impulses" 
	 		-> {n = Impulse.ar(400)*SinOsc.ar(1).range(0,1); Trig1.ar(n,0) - (n>0)},
	 "Trig1.ar(_,0) is no-op when applied to Impulse.ar" -> {Impulse.ar(300) - Trig1.ar(Impulse.ar(300), 0)},
	 "Latch applied to LFPulse.ar on its own changes is no-op" -> {n=LFPulse.ar(23, 0.5); n - Latch.ar(n, HPZ1.ar(n).abs)},
	 "Latch applied to LFPulse.kr on its own changes is no-op" -> {n=LFPulse.kr(23, 0.5); n - Latch.kr(n, HPZ1.kr(n).abs)},
	 "Gate applied to LFPulse.ar on its own changes is no-op" -> {n=LFPulse.ar(23, 0.5); n - Gate.ar(n, HPZ1.ar(n).abs)},
	 "Gate applied to LFPulse.kr on its own changes is no-op" -> {n=LFPulse.kr(23, 0.5); n - Gate.kr(n, HPZ1.kr(n).abs)},
	 
	 //////////////////////////////////////////
	 // Linear-to-exponential equivalences:
	 "XLine.ar == Line.ar.linexp" -> {XLine.ar(0.01, 10, 1) - Line.ar( 0, 1, 1).linexp(0,1, 0.01, 10)},
	 "XLine.kr == Line.kr.linexp" -> {XLine.kr(0.01, 10, 1) - Line.kr( 0, 1, 1).linexp(0,1, 0.01, 10)},
	 "XLine.ar == Line.ar.exprange" -> {XLine.ar(0.01, 10, 1) - Line.ar(-1, 1, 1).exprange(   0.01, 10)},
	 "XLine.kr == Line.kr.exprange" -> {XLine.kr(0.01, 10, 1) - Line.kr(-1, 1, 1).exprange(   0.01, 10)},
	 "Line.ar == XLine.ar.explin" -> {Line.ar(0, 1, 1) - XLine.ar(0.01, 10, 1).explin(0.01, 10, 0, 1)},
	 "Line.kr == XLine.kr.explin" -> {Line.kr(0, 1, 1) - XLine.kr(0.01, 10, 1).explin(0.01, 10, 0, 1)},
	 
	 //////////////////////////////////////////
	 // Trigonometric:
	 "SinOsc.ar can match Line.ar.sin" -> {SinOsc.ar(1) - Line.ar(0,2pi,1).sin},
	 "SinOsc.kr can match Line.kr.sin" -> {SinOsc.kr(1) - Line.kr(0,2pi,1).sin},
	 "SinOsc.ar can match Line.ar.cos" -> {SinOsc.ar(1, pi/2) - Line.ar(0,2pi,1).cos},
	 "SinOsc.kr can match Line.kr.cos" -> {SinOsc.kr(1, pi/2) - Line.kr(0,2pi,1).cos},
	 "atan undoes tan" -> {n = WhiteNoise.ar; n - n.tan.atan},
	 "EnvGen.ar can recreate SinOsc with piecewise sin envelope" -> {EnvGen.ar(Env([-1,1,-1],[0.5,0.5],'sin')) - SinOsc.ar(1, -pi/2)},
	 
	 //////////////////////////////////////////
	 // Simple scaling and multiply-adds:
	 "(_+1)*2 == _.madd(2,2)" -> {n=WhiteNoise.ar; ((n+1)*2) - (n.madd(2,2)) },
	 "(_+1)*2 == _.madd(2,2)" -> {n=WhiteNoise.kr; ((n+1)*2) - (n.madd(2,2)) },
	 // NOTE: .pow(2) is unconventional in producing neg values on neg inputs (hence use .abs below). It's weird but intentional:
	 "_.pow(2).abs == _ * _" -> {n=WhiteNoise.ar; n.pow(2).abs - (n*n) },
	 "_.pow(2).abs == _ * _" -> {n=WhiteNoise.kr; n.pow(2).abs - (n*n) },
	 // DC scaling and K2A:
	 "DC equivalence" -> {DC.ar(2) - K2A.ar(DC.kr(1)) - 1 },
	 "sum and rescale ar signal is identity" -> {n=WhiteNoise.ar; [n, n].sum.madd(0.5, 0) - n },
	 "sum and rescale kr signal is identity" -> {n=WhiteNoise.kr; [n, n].sum.madd(0.5, 0) - n },
	 
	 //////////////////////////////////////////
	 // Panners (linear panners easy to verify - sum should recover original):
	 // FAILS on sc 3.3.1 - first 64 samples don't seem to pan as intended, upon first run. Subsequent runs OK - uses unintialised memory?:
	 "LinPan2.sum is identity (<=3.3.1 fails this)" -> {n=WhiteNoise.ar; LinPan2.ar(n, Line.kr(-1,1,1)).sum - n },
	 // These next two verify the fix I applied to LinPan2's constructor, revealed by the above. So 3.3.1 will also fail these:
	 "LinPan2_aa's action can be replicated by manually modulating amplitude (<=3.3.1 fails this)" ->
	 		 {n=DC.ar(1); v=Line.ar(Rand(),Rand(),1); LinPan2.ar(n, v)[1]*2-1 - v },
	 "LinPan2_ak's action can be replicated by manually modulating amplitude (<=3.3.1 fails this)" ->
	 		 {n=DC.ar(1); v=Line.kr(Rand(),Rand(),1); LinPan2.ar(n, v)[1]*2-1 - v },

	 //////////////////////////////////////////
	 // Peak-followers etc:
	 "Peak.ar on increasing pos signal is identity" -> {n=Line.ar(0,1,1); Peak.ar(n) - n },
	 "Peak.kr on increasing pos signal is identity" -> {n=Line.kr(0,1,1); Peak.kr(n) - n },
	 "Amplitude.ar on increasing pos signal (w sharp attack) is identity" -> {n=Line.ar(0,1,1); Amplitude.ar(n,0,1) - n },
	 "Amplitude.kr on increasing pos signal (w sharp attack) is identity" -> {n=Line.kr(0,1,1); Amplitude.kr(n,0,1) - n },
	 "Amplitude.ar on decreasing pos signal (w sharp decay ) is identity" -> {n=Line.ar(1,0,1); Amplitude.ar(n,1,0) - n },
	 "Amplitude.kr on decreasing pos signal (w sharp decay ) is identity" -> {n=Line.kr(1,0,1); Amplitude.kr(n,1,0) - n },
	 
	 //////////////////////////////////////////
	 // Clipping and distortion:
	 ".clip(2) doesn't affect signals that lie within +-1" -> {n=WhiteNoise.ar;   n.clip2(1) - n},
	 ".clip(2) on a loud LFPulse is same as scaling" -> {n=LFPulse.ar(LFNoise0.kr(50), mul:100);   n.clip2(1) - (n/100)},
	 
	 //////////////////////////////////////////
	 // FFT:
	 "IFFT(FFT(_)) == Delay(_, buffersize-blocksize)" -> {n =  PinkNoise.ar(1,0,1); DelayN.ar(n, 1984*SampleDur.ir, 1984*SampleDur.ir) - IFFT(FFT(LocalBuf(2048), n))  },
	 "IFFT(FFT(_)) == Delay(_, buffersize-blocksize)" -> {n = WhiteNoise.ar(1,0,1); DelayN.ar(n, 4032*SampleDur.ir, 4032*SampleDur.ir) - IFFT(FFT(LocalBuf(4096), n))  },

	]
	.keysValuesDo{|name, func| func.loadToFloatArray(1, Server.default, { |data|
			this.assertArrayFloatEquals(data, 0, name.quote, within: 0.001, report: false)
		});
		0.12.wait;
	};
	
	
	1.6.wait; // enough time for 1-second generators to run and to report back.
}
} // end TestCoreUGens class
