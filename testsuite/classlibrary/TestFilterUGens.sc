TestFilterUGens : UnitTest {
	var server;

	setUp {
		server = Server(this.class.name);
		server.options.protocol = \tcp;
		server.bootSync;
	}

	tearDown {
		server.quit;
		server.remove;
	}

	// check time invariance of all subClasses of Filter, particularly during the first 64 samples.
	// Filter followed by delay should be the same as delay followed by filter.
	// Only uses the filters' default arguments, but this should catch most initialisation errors.
	test_time_invariance {
		var condvar = CondVar();

		// a list of filters to test.  Should basically be Filter.allsubclasses, minus a
		// few odd ones like DetectSilence, plus anything that's supposed to behave like a filter
		// but doesn't drive from Filter.  Will ignore missing classes, so can include filters
		// defined in extensions.
		var filters = [
			\CombFormlet,
			\InsideOut,
			\LagUD,
			\Decay,
			\OnePole,
			\MoogFF,
			\Integrator,
			\RLPF,
			\MedianTriggered,
			\MidEQ,
			\Slew,
			\TwoPole,
			\Formlet,
			\BPF,
			\LPF,
			\LPZ2,
			\Decay2,
			\Resonz,
		//	\Friction, // fixme
			\Lag,
			\LPZ1,
			\Ringz,
			\FOS,
			\MeanTriggered,
			\LeakDC,
			\Slope,
			\SOS,
			\Lag2UD,
			\Lag3UD,
			\OneZero,
			\RHPF,
			\TwoZero,
		//	\APF, // fixme
			\BRF,
			\HPF,
			\HPZ2,
			\BPZ2,
			\BRZ2,
			\Lag2,
		//	\Ramp, // fixme
			\Lag3,
			\HPZ1,
			\BHiPass,
			\BLowShelf,
			\BPeakEQ,
			\BBandPass,
			\BBandStop,
			\BAllPass,
			\BHiShelf,
			\BLowPass,
			/*
			\NestedAllpassN,
			\NestedAllpassC,
			\DoubleNestedAllpassN,
			\DoubleNestedAllpassL,
			\DoubleNestedAllpassC, // fix us
			*/
			\MoogLadder,
			\RLPFD,
			// \Streson, // fixme
		].collect(_.asClass).reject(_.isNil);

		var delay_times = [1,64]; // in samples
		var numTests = delay_times.size * filters.size;
		var completed = 0;
		var impFrq = 500;

		filters.do {
			arg filter;
			delay_times.do {
				arg delay_samples;

				var message = filter.asString ++ ".ar is time invariant when delayed by "
					++ delay_samples ++ " sample(s).";

				{
					var deltime = delay_samples/SampleRate.ir;
					// should be silent - FP rounding errors are ok.
					// Note: impulse phase offset to avoid errors associated with
					//  	 initialization sample bugs in filter UGens (and others).
					//       https://github.com/supercollider/rfcs/pull/19
					DelayN.ar(filter.ar(Impulse.ar(impFrq, 0.25)), deltime, deltime)
					- filter.ar(DelayN.ar(Impulse.ar(impFrq, 0.25), deltime, deltime));
					// DelayN.ar(filter.ar(Impulse.ar(0)), deltime, deltime)
					// - filter.ar(DelayN.ar(Impulse.ar(0), deltime, deltime));
				}.loadToFloatArray(2*impFrq.reciprocal, server, {
					arg data;
					this.assertArrayFloatEquals(data, 0, message, within:1e-10, report:true);
					completed = completed + 1;
					condvar.signalOne;
				});

			}
		};

		condvar.waitFor(1, { completed == numTests });

		"".postln;
		postln("Please note: the following subclasses of Filter are not included in TestFilterUgens.");
		postln("Please consider adding them unless there is a good reason not to");
		postln("(i.e. the class is not intended as a time invariant filter UGen).");
		Filter.allSubclasses.do {
			arg filter;
			if (filters.includes(filter).not) {
				filter.postln;
			}
		};
		"".postln;
	}

	test_Integrator_add_presample_once_only {
		var buffer;  // use a buffer to guarantee exactly 1 input value of 1
		var synth;
		var testResp;
		var result;
		var cond = CondVar.new;

		buffer = Buffer.alloc(server, 500, 1, { |buf| buf.zeroMsg });
		server.sync;
		buffer.set(0, 1);
		server.sync;

		synth = {
			var phasor = Phasor.ar(0, 1, 0, buffer.numFrames);
			// don't allow to loop
			var stop = Line.kr(0, 1, 0.01, doneAction: 2);
			var sig = Integrator.ar(BufRd.ar(1, buffer, phasor, loop: 0, interpolation: 1));
			SendReply.kr(Done.kr(stop), '/testIntegrator', A2K.kr(sig));
			Silent.ar(1)
		}.play(target: server);

		testResp = OSCFunc({ |msg|
			result = msg[3];
			cond.signalOne;
		}, '/testIntegrator', server.addr, argTemplate: [synth.nodeID]);

		cond.waitFor(0.5);
		testResp.free;
		buffer.free;

		this.assertEquals(result, 1, "Integral of a single 1 should be 1");
	}

	test_ugen_generator_equivalences {
		var condvar = CondVar();
		var completed = 0;

		// These functions should generate an output of zeros, by subracting "equivalent"
		// UGen networks. The tolerance var allows for floating point error.
		var tolerance = -100.dbamp;
		var testDur = 0.1; // duration rendered for each test

		var tests = Dictionary[

			"Decay2 is the difference of two Decays" -> {
				var att = 0.001, dec = 0.01;
				var in = Impulse.ar(30);
				Decay2.ar(in, att, dec) - (Decay.ar(in, dec) - Decay.ar(in, att));
			},

			// Add more equivalence tests here. See TestCoreUGens:test_ugen_generator_equivalences
		];

		server.bootSync;

		tests.keysValuesDo{ |name, func|
			func.loadToFloatArray(testDur, server, { |data|
				this.assertArrayFloatEquals(data, 0, name.quote, within: tolerance, report: true);
				completed = completed + 1;
				condvar.signalOne;
			});
		};

		condvar.waitFor(1, { completed == tests.size });
	}

}
