/*
s.boot;
UnitTest.gui
TestCoreUGens.run
*/
TestCoreUGens : UnitTest {

	classvar <>defaultNumBlocksToTest = 10;
	// The wait condition following the tests is set, by loose convension,
	// to timeout at (1.5 * testDur * numTests), which should be more than
	// enough to never time out when running asynchronous tests.
	// However, this can be problematic if, e.g., there is a single test
	// with a very short duration causing a very short timeout. So
	// minTimeOut ensures a minimum timeout time after the last test.
	classvar <>minTimeOut = 2;

	var server;

	setUp {
		server = Server(this.class.name);
	}

	tearDown {
		server.quit;
		server.remove;
	}

	test_ugen_generator_equivalences {
		var cond = CondVar();
		var completed = 0;
		var blockSize, sampleRate;
		var n, v, tests, testDur, numTests, testsFinished;
		var sampleDursToTest; // for Env/Line equivalence tests

		// These pairs should generate the same shapes, so subtracting should give zero.
		// Of course there's some rounding error due to floating-point accuracy.
		tests = Dictionary[
			//////////////////////////////////////////
			// Ramp generators:
			"Line.ar can match LFSaw.ar" -> {Line.ar(0,1,1) - LFSaw.ar(0.5)},
			"Line.kr can match LFSaw.kr" -> {Line.kr(0,1,1) - LFSaw.kr(0.5)},
			"Line can match crossfaded DC" -> {Line.ar(0,1,1) - LinXFade2.ar(DC.ar(0), DC.ar(1), Line.ar(-1,1,1))},
			// (Integrator goes a bit off ramp cos of roundoff error accumulations)
			"Line.ar can match integrated DC" -> {Line.ar(SampleDur.ir,1,1) - Integrator.ar(DC.ar(SampleDur.ir))},
			"Line.ar can match EnvGen.ar with slope Env" -> {Line.ar - EnvGen.ar(Env([0,1],[1]))},

			//////////////////////////////////////////
			// Triggers:
			"Trig.ar(_,0) is no-op when applied to Impulse.ar, whatever the amplitude of the impulses"
			-> {n = Impulse.ar(400)*SinOsc.ar(0.713,0.001).range(0,1); Trig.ar(n,0) - n},
			"Trig1.ar(_,0) has same effect as (_>0) on variable-amplitude impulses"
			-> {n = Impulse.ar(400)*SinOsc.ar(0.713,0.001).range(0,1); Trig1.ar(n,0) - (n>0)},
			"Trig1.ar(_,0) is no-op when applied to Impulse.ar" -> {Impulse.ar(300) - Trig1.ar(Impulse.ar(300), 0)},
			"Latch applied to LFPulse.ar on its own changes is no-op" -> {n=LFPulse.ar(23, 0.5); n - Latch.ar(n, HPZ1.ar(n).abs)},
			"Latch applied to LFPulse.kr on its own changes is no-op" -> {n=LFPulse.kr(23, 0.5); n - Latch.kr(n, HPZ1.kr(n).abs)},
			"Gate applied to LFPulse.ar on its own changes is no-op" -> {n=LFPulse.ar(23, 0.5); n - Gate.ar(n, HPZ1.ar(n).abs)},
			"Gate applied to LFPulse.kr on its own changes is no-op" -> {n=LFPulse.kr(23, 0.5); n - Gate.kr(n, HPZ1.kr(n).abs)},
			"T2A should recover an ar signal passed through T2K" -> {
				// note this freq has to be less than half critical sampling at control rate
				// so that the kr signal goes to zero between frames so it's interpreted
				// as a trigger by T2A (otherwise it's just a DC kr signal, which isn't a trigger)
				var trigFreq = server.sampleRate / (server.options.blockSize) / 2;
				var arTrig = Impulse.ar(trigFreq);
				arTrig - T2A.ar(T2K.kr(arTrig))
			},
			"T2A with offset should recover a delayed ar signal passed through T2K" -> {
				var trigFreq = server.sampleRate / (server.options.blockSize) / 2;
				var arTrig = Delay2.ar(Impulse.ar(trigFreq));
				arTrig - T2A.ar(T2K.kr(arTrig), offset: 2)
			},

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
			// FIXME
			// "Duty.ar(SampleDur.ir, 0, x) == x" -> {n=WhiteNoise.ar; (n - Duty.ar(SampleDur.ir, 0, n)) },
			// "Duty.ar(SampleDur.ir, 0, Drand([x],inf)) == x" -> {n=WhiteNoise.ar; (n - Duty.ar(SampleDur.ir, 0, Drand([n],inf))) },


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

		//////////////////////////////////////////
		// Delay1, Delay2
		// More test in test_delay1_2_predelayValues
		[\ar, \kr].do{ |rate|
			tests.add(
				("Delay1." ++ rate ++ " first frame is 0 at .ar and matches the input at .kr") -> {
					var dc, imp, target;
					dc = DC.perform(rate, 1);       // all 1's
					imp = Impulse.perform(rate, 0); // single impulse on first sample
					target = switch(rate,
						\ar, dc - imp,              // subtracted impulse zeros out the the first sample of DC(1)
						\kr, dc
					);
					// target is equivalent to DC(1) delayed by 1 sample
					Delay1.perform(rate, dc) - target
				}
			);
			tests.add(
				("Delay2." ++ rate ++ " first 2 frames are 0 at .ar and match the input at .kr") -> {
					// as Delay1, but with two consecutive impulses
					// to cancel the first two samples of DC(1)
					var dc, imp, delimp, target;
					dc = DC.perform(rate, 1);           // all 1's
					imp = Impulse.perform(rate, 0);     // single impulse on first sample
					delimp = Delay1.perform(rate, imp); // single impulse on second sample
					target = switch(rate,
						\ar, dc - imp - delimp,         // subtracted impulse zeros out the the first sample of DC(1)
						\kr, dc
					);
					// target is equivalent to DC(1) delayed by 2 samples
					Delay2.perform(rate, dc) - target
				}
			);
		};

		//////////////////////////////////////////
		// reversible unary ops:
		[
			[\reciprocal, \reciprocal],
			[\squared, \sqrt],
			[\cubed, { |x| x ** (1/3) }],
			[\exp, \log],
			[\midicps, \cpsmidi], // this test currently limits passing tolerance for all tests
			[\midiratio, \ratiomidi],
			[\dbamp, \ampdb],
			[\octcps, \cpsoct],
			[\sin, \asin],
			[\cos, \acos],
			[\tan, \atan]
		].do { |selectors|
			[selectors, selectors.reverse].do { |pair|
				[\ar, \kr].do{ |rate|
					tests.add(
						"x == %(%(x)) [%]".format(pair[0], pair[1], rate) -> {
							var n = WhiteNoise.perform(rate).range(0.3, 0.9);
							n - pair[1].applyTo(pair[0].applyTo(n))
						}
					);
				}
			}
		};

		//////////////////////////////////////////
		// delays/bufdelays:
		[
			[DelayN, BufDelayN],
			[DelayL, BufDelayL],
			// [DelayC, BufDelayC] // not equivalent, FIXME
		].do { |classes|
			[\ar, \kr].do{ |rate|
				tests.add(
					"% == % [%]".format(classes[0], classes[1], rate) -> {
						var sig = SinOsc.perform(rate) + 1;
						var delayTime = WhiteNoise.perform(rate).range(0, 0.005);
						var delay = classes[0].perform(rate, sig, 0.02, delayTime);
						var bufdelay = classes[1].perform(rate, LocalBuf.new(0.02 * SampleRate.ir * 2), sig, delayTime);
						delay - bufdelay
					}
				);
			}
		};

		//////////////////////////////////////////
		// Run tests:
		server.bootSync;
		blockSize = server.options.blockSize;
		sampleRate = server.sampleRate;
		testDur = defaultNumBlocksToTest * blockSize / sampleRate;
		numTests = tests.size;

		tests.keysValuesDo { |name, func, i|
			func.loadToFloatArray(testDur, server, { |data|
				this.assertArrayFloatEquals(data, 0, name.quote,
					within: -90.dbamp, // could go to -100 if not for midicps(cpsmidi()) test
					report: false);
				completed = completed + 1;
				cond.signalOne;
			});
			server.sync;
		};

		testsFinished = cond.waitFor((1.5 * numTests * testDur).max(minTimeOut), { completed == numTests });
		this.assert(testsFinished, "TIMEOUT: ugen_generator_equivalences tests", report: false);


		// Other equivalence tests that don't use the batch test parameters above:

		//////////////////////////////////////////
		// Equivalence of linear Env, Line and XLine, with short duration and fast phase change.
		// In response to errors found in "m_grow" (step size) calculation in EnvGen,
		// in particular with small env segment sizes.
		sampleDursToTest = (2 .. 15);
		numTests = sampleDursToTest.size * 4; // * 4: tests performed on Line and XLine, at ar and kr
		completed = 0; // reset counter

		[\ar, \kr].do { |rate|
			sampleDursToTest.do{ |numSamps|
				var sampleDur = switch(rate,
					\ar, { 1 / sampleRate },
					\kr, { blockSize / sampleRate }
				);
				var lineDur = numSamps * sampleDur;

				{   // ensure values change enough to meaningfully exceed 'within' threshold
					EnvGen.perform(rate, Env([1.5, 100], lineDur)) -
					Line.perform(rate, 1.5, 100, lineDur)
				}.loadToFloatArray(testDur, server, { |data|
					this.assertArrayFloatEquals(data, 0,
						"EnvGen should be equal to Line over short durations [% samples] with fast phase change. [%]".format(numSamps, rate),
						within: -90.dbamp, report: false
					);
					completed = completed + 1;
					cond.signalOne;
				});
				server.sync;

				if (numSamps == 2) {
					// When duration is 2 samples, skip EnvGen vs. XLine test.
					// In this exceptional case, EnvGen is forced to a linear shape (could probably be fixed).
					completed = completed + 1;
					cond.signalOne;
				} {
					{   // ensure values change enough to meaningfully exceed 'within' threshold
						EnvGen.perform(rate, Env([1.5, 100], lineDur, \exp)) -
						XLine.perform(rate, 1.5, 100, lineDur)
					}.loadToFloatArray(testDur, server, { |data|
						this.assertArrayFloatEquals(data, 0,
							"EnvGen should be equal to XLine over short durations [% samples] with fast phase change. [%]".format(numSamps, rate),
							within: -90.dbamp, report: false
						);
						completed = completed + 1;
						cond.signalOne;
					});
				};
				server.sync;
			}
		};

		testsFinished = cond.waitFor(
			(1.5 * numTests * sampleDursToTest.maxItem / server.sampleRate).max(minTimeOut),
			{ completed == numTests }
		);
		this.assert(testsFinished, "TIMEOUT: ugen_generator_equivalences EnvGen/Line tests", report: false);
	}

	test_exact_convergence {
		var cond = CondVar();
		var completed = 0;
		var n, v, tests, testDur, testsFinished;

		// Tests for things that should converge exactly to zero
		tests = Dictionary[

			//////////////////////////////////////////
			// Pan2 amplitude convergence to zero test, unearthed by JH on sc-dev 2009-10-19.
			"Pan2.ar(ar, 0, kr) should converge properly to zero when amp set to zero" -> {
				(Line.ar(1,0,0.2)<=0)*Pan2.ar(BrownNoise.ar, 0, Line.kr(1,0, 0.1)>0).mean
			},

			// add more tests here
		];

		server.bootSync;
		testDur = defaultNumBlocksToTest * server.options.blockSize / server.sampleRate;

		tests.keysValuesDo{ |text, func|
			func.loadToFloatArray(testDur, server, { |data|
				this.assertArrayFloatEquals(data, 0, text.quote, within: 0.0, report: false);
				completed = completed + 1;
				cond.signalOne;
			});
			server.sync;
		};

		testsFinished = cond.waitFor((1.5 * tests.size * testDur).max(minTimeOut), { completed == tests.size });
		this.assert(testsFinished, "TIMEOUT: exact_convergence tests", report: false);
	}


	test_muladd {
		var cond = CondVar();
		var completed = 0;
		var n, v, tests, testDur, testsFinished;

		tests = Dictionary[];
		[[\ar,\kr], [2,0,5], [\ar,\kr], [2,0,5], [\ar,\kr], [2,0,5]].allTuples.do{|tup|
			tests["%%.madd(%%, %%)".format(*tup)] =
			"{DC.%(%).madd(DC.%(%), DC.%(%)) - (% * % + %)}".format(*(tup ++ tup[1,3..])).interpret;
		};

		server.bootSync;
		testDur = defaultNumBlocksToTest * server.options.blockSize / server.sampleRate;

		tests.keysValuesDo{|name, func|
			func.loadToFloatArray(testDur, server, { |data|
				this.assertArrayFloatEquals(data, 0, name.quote, report: false);
				completed = completed + 1;
				cond.signalOne;
			});
			server.sync;
		};

		testsFinished = cond.waitFor((1.5 * tests.size * testDur).max(minTimeOut), { completed == tests.size });
		this.assert(testsFinished, "TIMEOUT: muladd tests", report: false);
	}


	test_bufugens {
		var testsFinished;
		var tests = [1, 2, 8, 16, 32, 33];
		var cond = CondVar();
		var completed = 0;
		var bufDur = 0.2;

		server.bootSync;

		// channel sizes for test:
		tests.do{ |numchans|
			var data, dataBuf, recBuf;
			// random data for test
			data = { 1.0.rand }.dup((server.sampleRate * bufDur).round * numchans);
			// load data to server
			dataBuf = Buffer.loadCollection(server, data, numchans);
			// a buffer for recording the results
			recBuf = Buffer.alloc(server, data.size / numchans, numchans);
			server.sync;

			// copying data from dataBuf to recBuf:
			{
				RecordBuf.ar(PlayBuf.ar(numchans, dataBuf, BufRateScale.ir(dataBuf), doneAction: 2), recBuf, loop: 0) * 0.1;
			}.play(server, outbus: server.options.numInputBusChannels + server.options.numOutputBusChannels); // avoid hw output: write to first virutal bus

			server.sync;

			fork {
				(bufDur * 1.05).wait; // wait for the buffer to fill up
				recBuf.loadToFloatArray(action: { |renderedData|
					// the data recorded to renderedData should be exactly the same as the original data
					this.assertArrayFloatEquals(renderedData - data, 0,
						"data->loadCollection->PlayBuf->RecordBuf->loadToFloatArray->data (% channels)".format(numchans), report: false);
					dataBuf.free;
					recBuf.free;
					completed = completed + 1;
					cond.signalOne;
				});
			};
		};

		testsFinished = cond.waitFor((1.5 * tests.size * bufDur).max(minTimeOut), { completed == tests.size });
		this.assert(completed == tests.size, "TIMEOUT: bufugens tests", report: false);
	}

	test_impulse {
		var blockRate, funcs, results, frq, phs, testDur, testsFinished;
		var rates = [\kr,\ar];
		var cond = CondVar();
		var numBlocksToTest = 5;

		server.bootSync;

		blockRate = server.sampleRate / server.options.blockSize;
		frq = blockRate * 2.123; // impulses per block
		phs = 0;

		rates.do{ |rate|
			var completed = 0;

			funcs = [{DC.ar(frq)}, {DC.kr(frq)}, frq].collect({ |in0|
				[{DC.ar(phs)}, {DC.kr(phs)}, phs].collect{ |in1|
					{ Impulse.perform(rate, in0, in1) }
				}
			}).flat;
			results = Array.newClear(funcs.size);

			testDur = numBlocksToTest * blockRate.reciprocal;

			funcs.do{ |func, i|
				func.loadToFloatArray(testDur, server, { |arr|
					results[i] = arr;
					completed = completed + 1;
					cond.signalOne;
				});
				server.sync;
			};
			testsFinished = cond.waitFor((1.5 * funcs.size * testDur).max(minTimeOut), { completed == funcs.size });
			this.assert(testsFinished, "TIMEOUT: impulse tests", report: false);

			this.assert(results.every(_ == results[0]),
				"Impulse.%: all rate combinations of identical unmodulated input values should have identical output".format(rate),
				report: false);
		};

		/* Tests in response to historical bugs */

		// Phase wrapping, initial phase offset
		// https://github.com/supercollider/supercollider/pull/2864#issuecomment-299860789
		frq = 50;
		{ Impulse.kr(frq, 3.8) }.loadToFloatArray(
			frq.reciprocal, // render one freq period (should contain only 1 impulse)
			server, { |arr|
				this.assert(arr.sum == 1.0, "Impulse.kr: phase that is far out-of-range should wrap immediately in-range, and not cause multiple impulses to fire.", report: false);
				this.assert(arr[0] != 1.0, "Impulse.kr: a phase offset other than 0 or 1 should not produce an impulse on the first output sample.", report: false);
				cond.signalOne;
			}
		);
		cond.wait;

		// Phase offset of 0,1,-1 should be equal on first sample
		rates.do{ |rate|
			var phases = [0, 1, -1];
			phases.do{ |phs|
				{ Impulse.perform(rate, frq, phs) }.loadToFloatArray(
					frq.reciprocal, // 1 freq period
					server, { |arr|
						this.assert(arr[0] == 1.0,
							"Impulse.%: initial phase of % should produce and impulse on the first frame.".format(rate, phs), report: false);
						cond.signalOne;
					}
				);
				cond.wait;
			}
		};

		// Freq = 0 should produce a single impulse on first frame
		// https://github.com/supercollider/supercollider/pull/4150#issuecomment-582905976
		rates.do{ |rate|
			{ Impulse.perform(rate, 0) }.loadToFloatArray(
				3 * server.options.blockSize / server.sampleRate, // 3 blocks
				server, { |arr|
					this.assert(arr[0] == 1.0 and: { arr.sum == 1.0 },
						"Impulse.%: freq = 0 should produce a single impulse on the first frame and no more.".format(rate), report: false);
					cond.signalOne;
				}
			);
			cond.wait;
		};

		// Positive and negative freqs should produce the same output
		rates.do{ |rate|
			{ Impulse.perform(rate, 100 * [1,-1]) }.loadToFloatArray(
				5 * frq.reciprocal,
				server, { |arr|
					arr = arr.clump(2).flop; // de-interleave
					this.assertArrayFloatEquals(arr[0], arr[1],
						"Impulse.%: positive and negative frequencies should produce the same output.".format(rate), report: false);
					cond.signalOne;
				}
			);
			cond.wait;
		};
	}

	test_demand {
		var cond = CondVar();
		var nodesToFree, tests, testNaN;
		var dutyDur, seq;

		server.bootSync;
		nodesToFree = [];

		OSCFunc({ |message|
			if(nodesToFree.indexOf(message[1]).notNil) {
				nodesToFree.removeAt(nodesToFree.indexOf(message[1]))
			};
			nodesToFree.postln;
		}, \n_end, server.addr).oneShot;

		dutyDur = 0.05;
		seq = (1..5);
		tests = [
			{LPF.ar(LeakDC.ar(Duty.ar(dutyDur, 0, Dseq(seq), doneAction: 2)))}
		];

		tests.do{|item| nodesToFree = nodesToFree.add(item.play(server).nodeID) };

		server.sync;
		(dutyDur * seq.size).wait;

		// The items should all have freed by now...
		this.assert(nodesToFree.size == 0, "Duty should free itself after a limited sequence");

		// Test for nil - reference: "cmake build system: don't enable -ffast-math for gcc-4.0"
		testNaN = false;

		OSCFunc({ |message|
			switch(message[2],
				5453, { testNaN = message[3] <= 0.0 or: { message[3] >= 1.0 } }
			);
			cond.signalOne;
		}, \tr, server.addr).oneShot;

		{
			Line.kr(1, 0, dur: 1, doneAction: 2);
			SendTrig.kr(Impulse.kr(10, 0.5), 5453, LFTri.ar(Duty.ar(dutyDur, 0, Dseq(#[100], 1))))
		}.play(server);
		server.sync;
		cond.wait;

		this.assert(testNaN.not, "Duty+LFTri should not output NaN");
	}

	test_pitchtrackers {
		var tests, testsFinished;
		var cond = CondVar();
		var completed = 0;
		var fromFreq = 100;
		var toFreq = 4000;
		// a shorter duration requires higher error threshold for Pitch (faster phase/freq change)
		var testDur = 1.5;

		tests = Dictionary[
			"ZeroCrossing.ar() tracking a SinOsc" -> {
				var freq = XLine.ar(fromFreq, toFreq, testDur);
				var son = SinOsc.ar(freq);
				var val = ZeroCrossing.ar(son);
				var dev = (freq-val).abs;
				// gate the startup period of ZeroCrossing (just over one period of the input oscillator)
				var gate = ToggleFF.ar(TDelay.ar(Impulse.ar(0), dur: (fromFreq).reciprocal * 1.25));
				// normalize deviation by 10% of the target frequency
				dev = dev / (freq / 10);
				dev = dev * gate;
				// [freq, val, dev] // for plotting/debugging
			},
			"Pitch.kr() tracking a Saw" -> {
				var freq = XLine.kr(fromFreq, toFreq, testDur);
				var son = Saw.ar(freq);
				var val = Pitch.kr(son, initFreq: fromFreq).at(0);
				var dev = (freq-val).abs;
				// normalize deviation by 10% of the target frequency
				dev = dev / (freq / 10);
				// [freq, val, dev] // for plotting/debugging
			}//.plot(testDur, separately: true); // <-to observe/debug
		];

		server.bootSync;

		tests.keysValuesDo{ |text, func|
			func.loadToFloatArray(testDur, server, { |pitchDeviation|
				this.assertArrayFloatEquals(
					pitchDeviation, 0.0, text,
					within: 1.0 // deviation should be less than 1.0 (dev is normalized to 10% of target freq)
				);
				completed = completed + 1;
				cond.signalOne;
			});
			server.sync;
		};

		testsFinished = cond.waitFor((1.5 * testDur * tests.size).max(minTimeOut), { completed == tests.size });
		this.assert(testsFinished, "TIMEOUT: pitchtrackers tests", report: false);
	}

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

		server.bootSync;

		this.assert(testAudioRate.value, report:true, onFailure:"test_out_ugens: failed with audio rate ugens");
		this.assert(testControlRate.value, report:true, onFailure:"test_out_ugens: failed with control rate ugens");
	}

	test_delay1_2_predelayValues {
		var tolerance = -100.dbamp;
		var dcInputVal = 0.4;
		var testParams, testDur;

		testParams = [
			// [ugen, inputVal, x1 (, x2)]
			[Delay1, dcInputVal],       // default: no x1 arg specified
			[Delay1, dcInputVal, 0.5],

			[Delay2, dcInputVal],       // default: no x1 or x2 arg specified
			[Delay2, dcInputVal, 0.3],  // no x2 specified
			[Delay2, dcInputVal, 0.3, 0.6],
		];

		server.bootSync;
		testDur = defaultNumBlocksToTest * server.options.blockSize / server.sampleRate;

		testParams.do{ |params|
			var ugen, inVal, xArgs, numPredelaySamples;
			#ugen, inVal = params[0..1];
			xArgs = params[2..];
			numPredelaySamples = switch(ugen, Delay1, { 1 }, Delay2, { 2 });

			[\ar, \kr].do{ |rate|
				var cond = CondVar();
				{
					ugen.perform(rate,
						DC.perform(rate, inVal), 1, 0, *xArgs // in, mul, add, x1 (, x2)
					)
				}.loadToFloatArray(testDur, server, { |data|
					var errmsg, target;

					if(xArgs.size == 0) {
						// no args specified, check defaults
						switch(rate,
							\ar, {
								target = 0.0 ! numPredelaySamples;
								errmsg = "%.ar: predelay sample% should default to 0.0.";
							},
							\kr, {
								target = inVal ! numPredelaySamples;
								errmsg = "%.kr: predelay sample% should defaults to the input value (%).";
							}
						);
						errmsg = errmsg.format(ugen, if(ugen == Delay2){ "s" }{ "" }, inVal);
					} {
						// x1/x2 args are specified, partially or fully
						target = switch(rate,
							\ar, { xArgs.extend(numPredelaySamples, 0.0).reverse }, // reverse: output order is [x2, x1]
							\kr, { xArgs.extend(numPredelaySamples, inVal).reverse }
						);
						errmsg = format(
							"%.%: when x1% (%) is a number, the corresponding predelay sample equals that number.",
							ugen, rate, if(ugen == Delay2){ " or x2" }{ "" }, xArgs, inVal
						);
					};

					this.assertArrayFloatEquals(
						data.keep(numPredelaySamples), target, errmsg, within: tolerance, report: false
					);
					cond.signalOne;
				});

				cond.wait;
			};
		}
	}

	test_binaryValue_isUniform {
		var from = [100, -100, 0, { 100 }, { -100 }, { 0 }];
		var to = [1, 0, 0, 1, 0, 0];
		var text = ["positive", "negative", "zero", "positive valued function", "negative valued function", "zero valued function"];
		var cond = CondVar();
		var testDur;

		server.bootSync;
		testDur = defaultNumBlocksToTest * server.options.blockSize / server.sampleRate;

		from.size.do { |i|
			this.assert(from[i].binaryValue.value == to[i], "% should correspond to %".format(text[i], to[i]));
		};

		from.size.do { |i|

			{ DC.ar(from[i]).binaryValue }.loadToFloatArray(testDur, server, { |data|
				this.assertFloatEquals(data[0], to[i],"% signal should correspond to %".format(text[i], to[i]), within: 0.1, report: false);
				cond.signalOne;
			});
			cond.wait;
		};
	}

	// Test for correct arrival time of Envs with numerous stages
	test_env_endsOnTime {
		var shapes = [\sin, \lin, \exp, \sqr, \cub, \welch, -2.2, \step, \hold];
		var rates = [\kr, \ar];
		var numTests = shapes.size * rates.size;
		var cond = CondVar();
		var testsFinished = false;
		var completed = 0;

		// Many env stages to test time error accumulation
		// (10, 83.4 .. 1000).scramble ->
		var levels = [230.2, 817.4, 670.6, 890.8, 303.6, 377.0, 964.2, 523.8, 10.0, 156.8, 83.4, 597.2, 450.4, 744.0];

		// Between 3 and 6 samples per stage (ensure minimum of 2)
		// { rrand(3, 6) } ! (levels.size-1) ->
		var timesInSamps = [4, 5, 4, 6, 4, 4, 6, 3, 6, 6, 6, 4, 6];

		// Tolerance is relative to max possible level jump
		var tolerance = (levels.maxItem - levels.minItem) * -100.dbamp;

		server.bootSync;

		shapes.do{ |shape|
			rates.do{ |rate|
				var fs = switch(rate,
					\ar, { server.sampleRate },
					\kr, { server.sampleRate / server.options.blockSize }
				);
				// We expect the env to arrive at exactly this sample index.
				var envDur_smp = timesInSamps.sum.asInteger;

				// Render a few excess samples to observe position of Env's end value.
				// Must be >1 to avoid loadToFloatArray truncating last sample (floating point rounding error)
				var excessSamples = 4;
				var renderDur = (envDur_smp + excessSamples) / fs;

				{
					EnvGen.perform(rate,
						Env(levels, timesInSamps.normalizeSum, shape),
						timeScale: envDur_smp / fs
					);
				}.loadToFloatArray(renderDur, server,
					action: { |data|
						var test1, test2, test3;

						// the end val is reached after the total env duration has elapsed,
						// which is index envDur_smp
						var dataEndVal = data[envDur_smp];
						var dataEndVal_p1 = data[envDur_smp+1];
						var dataEndVal_m1 = data[envDur_smp-1];
						var envEndVal = levels.last;

						// store local vars to avoid async overwrites
						var shp = shape;
						var rt = rate;

						// Test 1: end value is correct
						test1 = ((dataEndVal - envEndVal).abs < tolerance);

						// Test 2: one before end value should not equal the end value
						// An exception to this test would be if the duration of the Line is
						// less than a frame duration. In that (odd) case the "correct"
						// first value is the end value. See GH Issue #4279.
						test2 = (dataEndVal - dataEndVal_m1).abs > tolerance;
						// test 2 doesn't apply to step or hold shapes
						if (shp == \step or: { shp == \hold }) { test2 = true };

						// Test 3: one sample after the env dur is the end val
						test3 = (dataEndVal_p1 - envEndVal).abs < tolerance;

						// We're more forgiving with \hold - it might arrive to its segment
						// value at envEndVal or endVal+1 depending on whether the sampling point
						// falls before or after the segment duration (there's a theoretical
						// "correct" answer, which would go against the how the docs say it behaves,
						// but modifying this would make a breaking change)
						if (shp == \hold and: { test3 or: { test1 } }) { test1 = test3 = true };

						this.assert(test1,
							format(
								"EnvGen should end at the correct value (%, %) - should be: % is: %\n",
								shp, rt, envEndVal, dataEndVal
							), report: false
						);
						this.assert(test2, format("EnvGen should not end early. (%, %)", shp, rt), report: false);
						this.assert(test3, format("EnvGen should not end late. (%, %)", shp, rt), report: false);

						// Test 4: While we're here, let's confirm EnvGen's first output sample is accurate.
						if (shp != \step) {
							this.assertFloatEquals(data.first, levels.first,
								format(
									"EnvGen's first output sample should be its given start value. (%, %)",
									shp, rt
								), report: false
							);
						};

						completed = completed + 1;
						cond.signalOne;
				});
				server.sync;
			} // end rates
		}; // end shapes

		// Conservatively set timeout based on the control rate tests' duration
		testsFinished = cond.waitFor(
			(1.5 * numTests * timesInSamps.sum * (server.sampleRate / server.options.blockSize)).max(minTimeOut),
			{ completed == numTests }
		);
		this.assert(testsFinished, "TIMEOUT: test_env_endsOnTime", report: false);
	}

	test_oscillators_startAtCorrectPhase {

		var tolerance = -100.dbamp;
		var cond = CondVar();

		server.bootSync;

		[\ar, \kr].do{ |rate|
			var period, period_sec, freq, halfPeriod, iPhase, maxIdx, target, ugen, phaseRange;
			var fs = switch(rate,
				\ar, { server.sampleRate },
				\kr, { server.sampleRate / server.options.blockSize }
			);

			period = 10; // oscillator period, in samples (should be even)
			period_sec = period / fs; // oscillator period, in seconds
			freq = fs / period;
			halfPeriod = period / 2;

			// LFPulse: initial phase is one sample before the end of the pulse width
			{
				iPhase = (halfPeriod - 1) / period; // LFPulse phase is 0->1
				LFPulse.perform(rate, freq, iPhase, 0.5)
			}.loadToFloatArray(period + 1 / fs, server, { |data|
				// exact float comparison should be ok here...
				// from LFPulse_next: z = phase < duty ? 1.f : 0.f;
				this.assert(data[0] == 1.0 and: { data[1] == 0.0 },
					"LFPulse.% should start at the correct initial phase".format(rate), report: false
				);
				cond.signalOne;
			});
			cond.wait;

			// LFSaw: initial phase is one sample before the signal flip toward -1
			{
				iPhase = (halfPeriod - 1) / period * 2; // LFPulse phase is 0->2
				LFSaw.perform(rate, freq, iPhase)
			}.loadToFloatArray(period + 1 / fs, server, { |data|
				this.assert(data[0] > 0.0 and: { data[1] < 0.0 },
					"LFSaw.% should start at the correct initial phase".format(rate), report: false
				);
				cond.signalOne;
			});
			cond.wait;

			// LFPar: test initial phase
			// [initPhase, firstValue] pairs to test, LFPar phase is 0->4
			[
				[0, 1.0],
				[1, 0.0],
				[2, -1.0],
				[3, 0.0],
				[4, 1.0]
			].do{ |pair|
				#iPhase, target = pair;
				{
					LFPar.perform(rate, freq, iPhase)
				}.loadToFloatArray(period + 1 / fs, server, { |data|
					this.assertFloatEquals(data[0], target,
						"LFPar.% should be % at initial phase of %".format(rate, target, iPhase),
						within: tolerance, report: false
					);
					cond.signalOne;
				});
				cond.wait;
			};

			// LFCub, LFTri, SinOsc: testing initial phase
			// [initPhase, firstValue] pairs to test — phase is normalized 0->1 here then scaled for each UGen
			[
				[0.00, 0.0],
				[0.25, 1.0],
				[0.50, 0.0],
				[0.75, -1.0],
				[1.00, 0.0]
			].do{ |pair|
				#iPhase, target = pair;
				[
					[LFCub, 2.0],  // LFPar phase is 0->2
					[LFTri, 4.0],  // LFTri phase is 0->4
					[SinOsc, 2pi], // SinOsc phase is 0->2pi
				].do{ |ugenPhaseRangePair|
					#ugen, phaseRange = ugenPhaseRangePair;
					{
						ugen.perform(rate, freq, iPhase * phaseRange)
					}.loadToFloatArray(period + 1 / fs, server, { |data|
						this.assertFloatEquals(data[0], target,
							"%.% should be % at initial phase of %".format(ugen, rate, target, iPhase * phaseRange),
							within: tolerance, report: false
						);
						cond.signalOne;
					});
					cond.wait;
				}
			};

			// VarSaw: testing initial phase
			period = 10; // ensure even
			freq = fs / period;
			[   // pairs of initial phase and corresponding index of the signal maximum
				[0.0, period / 2],  // max occurs in the middle
				[0.5, 0],           // max occurs at the beginning
				[1.0, period / 2]   // iphase:0.0 == iphase:1.0
			].do{ |pair|
				#iPhase, maxIdx = pair;
				{
					VarSaw.perform(rate, freq, iPhase, 0.5) // width must be 0.5 for this test
				}.loadToFloatArray(freq.reciprocal,         // must be one period only
					action: { |data|
						this.assertEquals(data.indexOf(data.maxItem), maxIdx,
							"VarSaw.% should start correct phase (initPhase: %, maxIdx: %)".format(rate, iPhase, maxIdx)
						);
						cond.signalOne;
					}
				);
				cond.wait;
			};

			// more phase tests here...

			// EnvGen, standard Env and using Env:*circle and Env:-circle
			target = 42; // first output sample should be the first level of the Env
			[
				{ EnvGen.perform(rate, Env([target, target*100], period_sec)) },
				// circular envs have to be instantiated from within the function in order for them to be interpreted as dynamic UGen inputs
				{ EnvGen.perform(rate, Env([target, target*100], period_sec).circle(period_sec)) },
				{ EnvGen.perform(rate, Env.circle([target, target*100], [period_sec, period_sec])) },
			].do{ |testFunc, i|
				testFunc.loadToFloatArray(period_sec * 1.5, server, { |data|
					this.assertFloatEquals(data[0], target,
						"EnvGen's first sample, should be its first level [%, env %]".format(rate, i),
						within: tolerance, report: false
					);
					cond.signalOne;
				});
				cond.wait;
			}
		}
	}


} // end TestCoreUGens class
