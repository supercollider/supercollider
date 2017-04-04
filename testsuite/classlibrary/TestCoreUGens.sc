/*
s.boot;
UnitTest.gui
TestCoreUGens.run
*/
TestCoreUGens : UnitTest {
	test_ugen_generator_equivalences {
		var n, v;

		// These pairs should generate the same shapes, so subtracting should give zero.
		// Of course there's some rounding error due to floating-point accuracy.
		var tests = Dictionary[
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

			// Audio rate demand ugens
			"Duty.ar(SampleDur.ir, 0, x) == x" -> {n=WhiteNoise.ar; (n - Duty.ar(SampleDur.ir, 0, n)) },
			"Duty.ar(SampleDur.ir, 0, Drand([x],inf)) == x" -> {n=WhiteNoise.ar; (n - Duty.ar(SampleDur.ir, 0, Drand([n],inf))) },


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
			"Amplitude.ar never non-negative (fixed in svn rev 9703)" -> {n=SinOsc.ar(440, -0.5pi); Amplitude.ar(n) < 0 },

			//////////////////////////////////////////
			// Clipping and distortion:
			".clip2() doesn't affect signals that lie within +-1" -> {n=WhiteNoise.ar;   n.clip2(1) - n},
			".clip2() on a loud LFPulse is same as scaling" -> {n=LFPulse.ar(LFNoise0.kr(50), mul:100);   n.clip2(1) - (n/100)},
			".clip2(_) == .clip(-_,_) (fixed in svn rev 9838)" -> {n=WhiteNoise.ar;   n.clip2(0.4) - n.clip(-0.4, 0.4)},
			"_.clip2().abs never greater than _.abs" -> {n=WhiteNoise.ar;   n.clip2(0.3).abs > n.abs },
			"_.clip( ).abs never greater than _.abs (fixed in svn rev 9838)" -> {n=WhiteNoise.ar;   n.clip(-0.7,0.6).abs > n.abs },

			//////////////////////////////////////////
			// FFT:
			"IFFT(FFT(_)) == Delay(_, buffersize-blocksize)" -> {n =  PinkNoise.ar(1,0,1); DelayN.ar(n, 1984*SampleDur.ir, 1984*SampleDur.ir) - IFFT(FFT(LocalBuf(2048), n))  },
			"IFFT(FFT(_)) == Delay(_, buffersize-blocksize)" -> {n = WhiteNoise.ar(1,0,1); DelayN.ar(n, 4032*SampleDur.ir, 4032*SampleDur.ir) - IFFT(FFT(LocalBuf(4096), n))  },

			//////////////////////////////////////////
			// CheckBadValues:
			"CheckBadValues.ar()" -> {
				var trig=Impulse.ar(10);
				var f=ToggleFF.ar(trig);
				var g=ToggleFF.ar(PulseDivider.ar(trig));
				var predicted = Demand.ar(trig,0,Dseq([2,0,0,1],inf));
				CheckBadValues.ar(f/g, post: 0) - predicted
			},
			"CheckBadValues.kr()" -> {
				var trig=Impulse.kr(10);
				var f=ToggleFF.kr(trig);
				var g=ToggleFF.kr(PulseDivider.kr(trig));
				var predicted = Demand.kr(trig,0,Dseq([2,0,0,1],inf));
				CheckBadValues.kr(f/g, post: 0) - predicted
			},

			//////////////////////////////////////////
			// Delay
			"DelayN" -> {	var sig = Impulse.ar(4);
				sig - DelayN.ar(sig, 1, 0.25) * EnvGen.kr(Env.new([0, 0, 1], [0.3, 0]))
			},
			"DelayN (audio rate delay time) " -> {	var sig = Impulse.ar(4);
				sig - DelayN.ar(sig, 1, DC.ar(0.25)) * EnvGen.kr(Env.new([0, 0, 1], [0.3, 0]))
			},
			"DelayL" -> {	var sig = Impulse.ar(4);
				sig - DelayL.ar(sig, 1, 0.25) * EnvGen.kr(Env.new([0, 0, 1], [0.3, 0]))
			},
			"DelayL (audio rate delay time) " -> {	var sig = Impulse.ar(4);
				sig - DelayL.ar(sig, 1, DC.ar(0.25)) * EnvGen.kr(Env.new([0, 0, 1], [0.3, 0]))
			},
			"DelayC" -> {	var sig = Impulse.ar(4);
				sig - DelayC.ar(sig, 1, 0.25) * EnvGen.kr(Env.new([0, 0, 1], [0.3, 0]))
			},
			"DelayC (audio rate delay time) " -> {	var sig = Impulse.ar(4);
				sig - DelayC.ar(sig, 1, DC.ar(0.25)) * EnvGen.kr(Env.new([0, 0, 1], [0.3, 0]))
			},
			"BufDelayN" -> { var sig = Impulse.ar(4);
				var buf = LocalBuf(SampleRate.ir * 1);
				sig - BufDelayN.ar(buf, sig, 0.25) * EnvGen.kr(Env.new([0, 0, 1], [0.3, 0]))
			},
			"BufDelayN (audio rate delay time) " -> { var sig = Impulse.ar(4);
				var buf = LocalBuf(SampleRate.ir * 1);
				sig - BufDelayN.ar(buf, sig, DC.ar(0.25)) * EnvGen.kr(Env.new([0, 0, 1], [0.3, 0]))
			},
			"BufDelayL" -> { var sig = Impulse.ar(4);
				var buf = LocalBuf(SampleRate.ir * 1);
				sig - BufDelayL.ar(buf, sig, 0.25) * EnvGen.kr(Env.new([0, 0, 1], [0.3, 0]))
			},
			"BufDelayL (audio rate delay time) " -> { var sig = Impulse.ar(4);
				var buf = LocalBuf(SampleRate.ir * 1);
				sig - BufDelayL.ar(buf, sig, DC.ar(0.25)) * EnvGen.kr(Env.new([0, 0, 1], [0.3, 0]))
			},
			"BufDelayC" -> { var sig = Impulse.ar(4);
				var buf = LocalBuf(SampleRate.ir * 1);
				sig - BufDelayC.ar(buf, sig, 0.25) * EnvGen.kr(Env.new([0, 0, 1], [0.3, 0]))
			},
			"BufDelayC (audio rate delay time) " -> { var sig = Impulse.ar(4);
				var buf = LocalBuf(SampleRate.ir * 1);
				sig - BufDelayC.ar(buf, sig, DC.ar(0.25)) * EnvGen.kr(Env.new([0, 0, 1], [0.3, 0]))
			},
			"DelTapRd " -> { var sig = Impulse.ar(4);
				var buf = LocalBuf(SampleRate.ir * 1);
				var phase = DelTapWr.ar(buf, sig);
				var delayed = DelTapRd.ar(buf, phase, 0.25);
				(sig - delayed) * EnvGen.kr(Env.new([0, 0, 1], [0.3, 0]))
			},
			"DelTapRd (audio rate delay time) " -> { var sig = Impulse.ar(4);
				var buf = LocalBuf(SampleRate.ir * 1);
				var phase = DelTapWr.ar(buf, sig);
				var delayed = DelTapRd.ar(buf, phase, DC.ar(0.25));
				(sig - delayed) * EnvGen.kr(Env.new([0, 0, 1], [0.3, 0]))
			},
			"DelTapRd (linear)" -> { var sig = Impulse.ar(4);
				var buf = LocalBuf(SampleRate.ir * 1);
				var phase = DelTapWr.ar(buf, sig);
				var delayed = DelTapRd.ar(buf, phase, 0.25, 2);
				(sig - delayed) * EnvGen.kr(Env.new([0, 0, 1], [0.3, 0]))
			},
			"DelTapRd (linear, audio rate delay time) " -> { var sig = Impulse.ar(4);
				var buf = LocalBuf(SampleRate.ir * 1);
				var phase = DelTapWr.ar(buf, sig);
				var delayed = DelTapRd.ar(buf, phase, DC.ar(0.25), 2);
				(sig - delayed) * EnvGen.kr(Env.new([0, 0, 1], [0.3, 0]))
			},
			"DelTapRd (cubic) " -> { var sig = Impulse.ar(4);
				var buf = LocalBuf(SampleRate.ir * 1);
				var phase = DelTapWr.ar(buf, sig);
				var delayed = DelTapRd.ar(buf, phase, 0.25, 4);
				(sig - delayed) * EnvGen.kr(Env.new([0, 0, 1], [0.3, 0]))
			},
			"DelTapRd (cubic, audio rate delay time) " -> { var sig = Impulse.ar(4);
				var buf = LocalBuf(SampleRate.ir * 1);
				var phase = DelTapWr.ar(buf, sig);
				var delayed = DelTapRd.ar(buf, phase, DC.ar(0.25), 4);
				(sig - delayed) * EnvGen.kr(Env.new([0, 0, 1], [0.3, 0]))
			},

		];
		var testsIncomplete = tests.size;


		//////////////////////////////////////////
		// reversible unary ops:

		[
			[\reciprocal, \reciprocal],
			[\squared, \sqrt],
			[\cubed, { |x| x ** (1/3) }],
			[\exp, \log],
			[\midicps, \cpsmidi],
			[\midiratio, \ratiomidi],
			[\dbamp, \ampdb],
			[\octcps, \cpsoct],
			[\sin, \asin],
			[\cos, \acos],
			[\tan, \atan]
		].do { |selectors|
			[selectors, selectors.reverse].do { |pair|
				tests = tests.add(
					"x == %(%(x)) [control rate]".format(*pair) -> {
						var n = WhiteNoise.kr.range(0.3, 0.9);
						n - pair[1].applyTo(pair[0].applyTo(n))
					}
				);
				tests = tests.add(
					"x == %(%(x)) [audio rate]".format(*pair) -> {
						var n = WhiteNoise.ar.range(0.3, 0.9);
						n - pair[1].applyTo(pair[0].applyTo(n))
					}
				)
			}
		};

		//////////////////////////////////////////
		// delays/bufdelays:

		[
			[DelayN, BufDelayN],
			[DelayL, BufDelayL],
			//	[DelayC, BufDelayC] // not equivalent, fixme
		].do { |classes|
			tests = tests.add(
				"% == % [control rate]".format(classes[0], classes[1]) -> {
					var sig = SinOsc.ar + 1;
					var delayTime = WhiteNoise.kr.range(0, 0.002);
					var delay = classes[0].ar(sig, 0.02, delayTime);
					var bufdelay = classes[1].ar(LocalBuf.new(0.02 * SampleRate.ir * 2), sig, delayTime);
					delay - bufdelay
				}
			);
			tests = tests.add(
				"% == % [audio rate]".format(classes[0], classes[1]) -> {
					var sig = SinOsc.ar + 1;
					var delayTime = WhiteNoise.ar.range(0, 0.002);
					var delay = classes[0].ar(sig, 0.02, delayTime);
					var bufdelay = classes[1].ar(LocalBuf.new(0.02 * SampleRate.ir * 2), sig, delayTime);
					delay - bufdelay
				}
			);
		};


		this.bootServer;
		tests.keysValuesDo{|name, func|
			func.loadToFloatArray(1, Server.default, { |data|
				this.assertArrayFloatEquals(data, 0, name.quote, within: 0.001, report: true);
				testsIncomplete = testsIncomplete - 1;
			});
			rrand(0.12, 0.35).wait;
		};


		this.wait{testsIncomplete==0};
	}

	test_exact_convergence {
		var n, v;

		// Tests for things that should converge exactly to zero
		var tests = Dictionary[
			//////////////////////////////////////////
			// Pan2 amplitude convergence to zero test, unearthed by JH on sc-dev 2009-10-19.
			"Pan2.ar(ar, , kr) should converge properly to zero when amp set to zero" -> {(Line.ar(1,0,0.2)<=0)*Pan2.ar(BrownNoise.ar, 0, Line.kr(1,0, 0.1)>0).mean},

		];
		var testsIncomplete = tests.size;
		this.bootServer;
		tests.keysValuesDo{|name, func|
			func.loadToFloatArray(1, Server.default, { |data|
				this.assertArrayFloatEquals(data, 0, name.quote, within: 0.0, report: true);
				testsIncomplete = testsIncomplete - 1;
			});
			rrand(0.05, 0.1).wait;
		};
		this.wait{testsIncomplete==0};
	}

	test_muladd {
		var n, v;
		var testsIncomplete;

		var tests = Dictionary[
		];
		[[\ar,\kr], [2,0,5], [\ar,\kr], [2,0,5], [\ar,\kr], [2,0,5]].allTuples.do{|tup|
			//tup.postln;
			tests["%%.madd(%%, %%)".format(*tup)] =
			"{DC.%(%).madd(DC.%(%), DC.%(%)) - (% * % + %)}".format(*(tup ++ tup[1,3..])).interpret;
		};

		testsIncomplete = tests.size;
		this.bootServer;
		tests.keysValuesDo{|name, func|
			func.loadToFloatArray(0.1, Server.default, { |data|
				this.assertArrayFloatEquals(data, 0, name.quote, report: true);
				testsIncomplete = testsIncomplete - 1;
			});
			rrand(0.06, 0.15).wait;
		};
		this.wait{testsIncomplete==0};
	}


	test_bufugens{
		var d, b, c;
		var testsIncomplete = 6;
		this.bootServer;

		// channel sizes for test:
		[1,2, 8,16, 32,33].do{ |numchans|

			// Random data for test
			d = {1.0.rand}.dup((Server.default.sampleRate * 0.25).round * numchans);

			// load data to server
			b = Buffer.loadCollection(Server.default, d, numchans);
			// a buffer for recording the results
			c = Buffer.alloc(Server.default, d.size / numchans, numchans);
			Server.default.sync;

			// Copying data from b to c:
			{
				RecordBuf.ar(PlayBuf.ar(numchans, b, BufRateScale.ir(b), doneAction: 2), c, loop:0) * 0.1;
			}.play;
			Server.default.sync;
			1.0.wait;
			c.loadToFloatArray(action: { |data|
				// The data recorded to "c" should be exactly the same as the original data "d"
				this.assertArrayFloatEquals(data - d, 0,
					"data->loadCollection->PlayBuf->RecordBuf->loadToFloatArray->data (% channels)".format(numchans), report: true);
				b.free;
				c.free;
				testsIncomplete = testsIncomplete - 1;
			});
			0.32.wait;
			Server.default.sync;
		};
		this.wait{testsIncomplete==0};
	}

	test_demand {
		var nodesToFree, tests, s = this.s, testNaN;

		this.bootServer;
		nodesToFree = [];

		OSCFunc({ |message|
			if(nodesToFree.indexOf(message[1]).notNil) {
				nodesToFree.removeAt(nodesToFree.indexOf(message[1]))
			};
		}, \n_end, s.addr).oneShot;

		tests = [
			{LPF.ar(LeakDC.ar(Duty.ar(0.1, 0, Dseq((1..8)), 2)))}
		];

		tests.do{|item| nodesToFree = nodesToFree.add(item.play.nodeID) };

		1.5.wait;
		s.sync;

		// The items should all have freed by now...
		this.assert(nodesToFree.size == 0, "Duty should free itself after a limited sequence");

		// Test for nil - reference: "cmake build system: don't enable -ffast-math for gcc-4.0"
		testNaN = false;

		OSCFunc({ |message|
			switch(message[2], 5453, { testNaN = message[3] <= 0.0 or: { message[3] >= 1.0 } });
		}, \tr, s.addr).oneShot;

		{
			Line.kr(1, 0, 1, 1, 0, 2);
			SendTrig.kr(Impulse.kr(10, 0.5), 5453, LFTri.ar(Duty.ar(0.1, 0, Dseq(#[100], 1))))
		}.play;
		1.5.wait;
		s.sync;
		this.assert(testNaN.not, "Duty+LFTri should not output NaN");
	}

	test_pitchtrackers {
		var tests = Dictionary[
			"ZCR.ar() tracking a SinOsc"
			-> { var freq = XLine.kr(100, 1000, 10);
				var son = SinOsc.ar(freq);
				var val = A2K.kr(ZeroCrossing.ar(son));
				var dev = (freq-val).abs * XLine.kr(0.0001, 1, 0.1);
				Out.ar(0, (son * 0.1).dup);
				dev},
			"Pitch.kr() tracking a Saw"
			-> { var freq = XLine.kr(100, 1000, 10);
				var son = Saw.ar(freq);
				var val = Pitch.kr(son).at(0);
				var dev = (freq-val).abs * XLine.kr(0.0001, 1, 0.1);
				Out.ar(0, (son * 0.1).dup);
				dev * 0.1 /* rescaled cos Pitch more variable than ZCR */ },
		];
		var testsIncomplete = tests.size;
		this.bootServer;
		tests.keysValuesDo{|text, func|
			func.loadToFloatArray(10, Server.default, { |data|
				this.assertArrayFloatEquals(data, 0.0, text, within: 1.0);
				testsIncomplete = testsIncomplete - 1;
			});
			rrand(0.12, 0.35).wait;
		};

		// Wait for async tests
		this.wait{testsIncomplete==0};
	} // test_pitchtrackers

	test_out_ugens {
		var testAudioRate, testControlRate;

		testControlRate = {
			var failing = [nil, []];
			var working = [0, [0], [0, 0, 0]];
			var stubs = [
				{ |args| LocalOut.kr(*args) },
				{ |args| Out.kr(0, *args) },
				{ |args| ReplaceOut.kr(0, *args) },
				{ |args| XOut.kr(0, 0.5, *args) },
			];

			var fails = failing.every { |input|
				stubs.every { |func|
					var fails = false;
					try { { func.(input) }.asSynthDef } { |err| fails = err.isException };
					if(fails.not) { "This is a case that should have failed:\n%\nOn this input:\n%\n".postf(func.cs, input) };
					fails
				}
			};
			var works = working.every { |input|
				stubs.every { |func|
					var works = true;
					try { { func.(input) }.asSynthDef } { |err| works = err.isException.not };
					if(works.not) { "This is a case that should not have failed:\n%\nOn this input:\n%\n".postf(func.cs, input) };
					works
				}
			};

			fails and: works
		};

		testAudioRate = {
			var failing = [nil, []];
			var working = [{DC.ar(0)}, {DC.ar([0, 0])}];
			var stubs = [
				{ |args| LocalOut.ar(*args) },
				{ |args| Out.ar(0, *args) },
				{ |args| OffsetOut.ar(0, *args) },
				{ |args| ReplaceOut.ar(0, *args) },
				{ |args| XOut.ar(0, 0.5, *args) },
			];

			var fails = failing.every { |input|
				stubs.every { |func|
					var fails = false;
					try { { func.(input) }.asSynthDef } { |err| fails = err.isException };
					if(fails.not) { "This is a case that should have failed:\n%\nOn this input:\n%\n".postf(func.cs, input) };
					fails
				}
			};
			var works = working.every { |input|
				stubs.every { |func|
					var works = true;
					try { { func.(input) }.asSynthDef } { |err| works = err.isException.not };
					if(works.not) { "This is a case that should not have failed:\n%\nOn this input:\n%\n".postf(func.cs, input) };
					works
				}
			};

			fails and: works
		};

		this.bootServer;
		this.assert(testAudioRate.value, report:true, onFailure:"test_out_ugens: failed with audio rate ugens");
		this.assert(testControlRate.value, report:true, onFailure:"test_out_ugens: failed with control rate ugens");

	}


} // end TestCoreUGens class
