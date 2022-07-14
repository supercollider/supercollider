TestFilterUGens : UnitTest {
	var server;

	setUp {
		server = Server(this.class.name);
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

		filters.do {
			arg filter;
			delay_times.do {
				arg delay_samples;

				var message = filter.asString ++ ".ar is time invariant when delayed by "
					++ delay_samples ++ " sample(s).";

				{
					var deltime = delay_samples/SampleRate.ir;
					// should be silent - FP rounding errors are ok.
					DelayN.ar(filter.ar(Impulse.ar(0)), deltime, deltime)
					- filter.ar(DelayN.ar(Impulse.ar(0), deltime, deltime));
				}.loadToFloatArray(0.1, server, {
					arg data;
					this.assertArrayFloatEquals(data, 0, message, within:1e-10, report:true);
					completed = completed + 1;
					condvar.signalOne;
				});

				rrand(0.012,0.035).wait;
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

}
