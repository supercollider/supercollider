TestUGen_initSample : UnitTest {

	var server;

	*initClass {
		passVerbosity = UnitTest.brief;
	}

	setUp {
		server = Server(this.class.name);
		server.bootSync;
	}

	tearDown {
		server.quit;
		server.remove;
	}

	// Async helpers

	awaitSynthOutput { |synthFn, runtimeSamples=1, timeout=1|
		var cond = CondVar(), synthOutput = nil, runtimeSeconds;
		// turn samples in blocks if synthFn is control-rate
		if (synthFn.asSynthDef.children.last.rate !== \audio) {
			runtimeSamples = runtimeSamples * server.options.blockSize
		};
		runtimeSeconds = runtimeSamples / server.sampleRate;
		// allow extra timeout for longer tests (e.g. testFFT with big memSize)
		if (runtimeSeconds > timeout) {
			timeout = runtimeSeconds + timeout
		};
		synthFn.loadToFloatArray(runtimeSeconds, server) { |out|
			synthOutput = out;
			cond.signalOne();
		};
		cond.waitFor(timeout) { synthOutput.notNil };
		^synthOutput;
	}

	assertSynthOutput { |synthFn, targetOutput, timeout=1|
		var nSamples = targetOutput.size;
		// var synthFnCode = synthFn.def.sourceCode.replaceRegexp("[ \n\r\t]","");
		var synthFnCode = synthFn.def.sourceCode;
		var out = this.awaitSynthOutput(synthFn, nSamples);
		if (out.isNil) {
			this.assert(false, "% timed out (timeout: %)".format(synthFnCode, timeout))
		} {

			this.assertArrayFloatEquals(out.keep(nSamples), targetOutput,
				"% should produce expected output".format(synthFnCode)
			);
		};
	}

	assertInitEqualsFirstSample { |synthFn|
		var out = this.awaitSynthOutput({
			var in = synthFn.value;
			var initSample = if (in.rate == \audio) {
				InitializationSample.ar(in)
			} {
				InitializationSample.ir(in)
			};
			[initSample, in]
		}, 1);
		var fnCode = synthFn.def.sourceCode.replaceRegexp("[ \n\r\t]","");
		this.assertFloatEquals(out[0], out[1],
			"% init sample and first sample should be equal".format(fnCode));
	}

	// Tests

	test_InitializationSample_initSample {
		this.assertSynthOutput(
			{ InitializationSample.ar(1) }, [1.0, 1.0],
			"InitializationSample"
		);
		this.assertSynthOutput(
			{ InitializationSample.ar(0) }, [0.0, 0.0],
			"InitializationSample"
		);
		this.assertInitEqualsFirstSample { InitializationSample.ar(1) };
		this.assertInitEqualsFirstSample { InitializationSample.ar(0) }
	}

	test_TriggerUGens_initSample {
		var testFirstInput = { |ugenClass|
			// testing assertInitEqualsFirstSample:
			// - test with a negative and a positive value as UGen first input
			// - test both .ar and .kr if the UGen supports them
			// - don't test mismatching rates, e.g. Trig1.ar(DC.kr(1234))
			var tests = [];
			if(ugenClass.respondsTo(\ar)) {
				tests = tests ++ ["{ %.ar(DC.ar(-1234)) }", "{ %.ar(DC.ar(1234)) }"]
			};
			if(ugenClass.respondsTo(\kr)) {
				tests = tests ++ ["{ %.kr(DC.kr(-1234)) }", "{ %.kr(DC.kr(1234)) }"]
			};
			tests.do { |fn|
				fn = fn.format(ugenClass).interpret;
				this.assertInitEqualsFirstSample(fn)
			}
		};

		testFirstInput.(Trig1);
		testFirstInput.(Trig);
		testFirstInput.(ToggleFF);
		testFirstInput.(SetResetFF);
		testFirstInput.(Latch);
		testFirstInput.(Gate);
		testFirstInput.(Schmidt);
		testFirstInput.(PulseDivider);
		testFirstInput.(PulseCount);

		testFirstInput.(Stepper);
		this.assertInitEqualsFirstSample({ Stepper.ar(DC.ar(1234), DC.ar(1)) });
		this.assertInitEqualsFirstSample({ Stepper.ar(DC.ar(1234), DC.ar(0)) });
		this.assertInitEqualsFirstSample({ Stepper.ar(DC.ar(-1234), DC.ar(1)) });
		this.assertInitEqualsFirstSample({ Stepper.ar(DC.ar(-1234), DC.ar(0)) });

		testFirstInput.(ZeroCrossing);
		testFirstInput.(TDelay);
		testFirstInput.(Timer);

		testFirstInput.(Sweep);
		// test Sweep_next_aa
		this.assertInitEqualsFirstSample({ Sweep.ar(DC.ar(1234), DC.ar(1)) });
		this.assertInitEqualsFirstSample({ Sweep.ar(DC.ar(-1234), DC.ar(1)) });

		testFirstInput.(Phasor);
		// test Phasor_next_aa
		this.assertInitEqualsFirstSample({ Phasor.ar(DC.ar(1234), DC.ar(1)) });
		this.assertInitEqualsFirstSample({ Phasor.ar(DC.ar(-1234), DC.ar(1)) });

		// test Peak and PeakFollower with negative input
		// because their output should be abs(in)
		testFirstInput.(Peak);
		this.assertInitEqualsFirstSample({ Peak.ar(DC.ar(-1)) });
		this.assertInitEqualsFirstSample({ Peak.kr(DC.kr(-1)) });
		testFirstInput.(PeakFollower);
		this.assertInitEqualsFirstSample({ PeakFollower.ar(DC.ar(-1)) });
		this.assertInitEqualsFirstSample({ PeakFollower.kr(DC.kr(-1)) });

		testFirstInput.(RunningMin);
		testFirstInput.(RunningMax);
		// test initial trig
		this.assertInitEqualsFirstSample({ RunningMin.ar(DC.ar(1), DC.ar(1)) });
		this.assertInitEqualsFirstSample({ RunningMin.ar(DC.ar(1), DC.kr(1)) });
		this.assertInitEqualsFirstSample({ RunningMax.ar(DC.ar(1), DC.ar(1)) });
		this.assertInitEqualsFirstSample({ RunningMax.ar(DC.ar(1), DC.kr(1)) });
		// need to test reset on trig
		// note: trigger is resetting on "next" sample: ask james harkins

		testFirstInput.(MostChange);
		testFirstInput.(LeastChange);
		testFirstInput.(LastValue);
		testFirstInput.(Done);

		// testFirstInput.(Pause);
		this.assertInitEqualsFirstSample({ Pause.kr(1, 9999) });
		this.assertInitEqualsFirstSample({ Pause.kr(0, 9999) });
		// testFirstInput.(Free);
		this.assertInitEqualsFirstSample({ Free.kr(1, 9999) });
		this.assertInitEqualsFirstSample({ Free.kr(0, 9999) });

		// These UGens are omitted because they return ^in
		// testFirstInput.(FreeSelfWhenDone);
		// testFirstInput.(PauseSelfWhenDone);
		// testFirstInput.(Poll);
		// testFirstInput.(FreeSelf);
		// testFirstInput.(PauseSelf);
		// These UGens are omitted because they return ^0.0
		// testFirstInput.(SendTrig);
		// testFirstInput.(SendReply);
		// testFirstInput.(SendPeakRMS);
	}

	test_TriggerUGens_initialOutput {

		// PulseDivider: test first trigger is not ignored
		this.assertSynthOutput(
			{ PulseDivider.ar(Impulse.ar(SampleRate.ir/2), 1) },
			[1,0,1,0,1,0,1,0]
		);
		// superfluous? PulseDivider has only one calcFunc
		this.assertSynthOutput(
			{ PulseDivider.kr(Impulse.kr(ControlRate.ir/2), 1) },
			[1,0,1,0,1,0,1,0]
		);

		// TDelay: test first trigger is delayed correctly
		this.assertSynthOutput({
			var sr = SampleRate.ir;
			TDelay.ar(Impulse.ar(sr/2), 1/sr)
		}, [0,1,0,1,0,1,0,1]);
		// superfluous? TDelay has only one calcFunc
		this.assertSynthOutput({
			var sr = ControlRate.ir;
			TDelay.kr(Impulse.kr(sr/2), 1/sr)
		}, [0,1,0,1,0,1,0,1]);

		// Timer: test first trigger is timed correctly
		this.assertSynthOutput({
			var sec = Timer.ar(Impulse.ar(SampleRate.ir/2));
			(sec * SampleRate.ir).round
		}, [0,0,2,2,2,2,2,2]);
		// superfluous? Timer has only one calcFunc
		this.assertSynthOutput({
			var sec = Timer.kr(Impulse.kr(ControlRate.ir/2));
			(sec * ControlRate.ir).round
		}, [0,0,2,2,2,2,2,2]);

		// Sweep: test reset on trig
		this.assertSynthOutput(
			{ Sweep.ar(Impulse.ar(SampleRate.ir/4), 1) * SampleRate.ir },
			[0, 1, 2, 3, 0, 1, 2, 3],
		);
		this.assertSynthOutput(
			{ Sweep.ar(Impulse.ar(SampleRate.ir/4), DC.ar(1)) * SampleRate.ir },
			[0, 1, 2, 3, 0, 1, 2, 3],
		);
		this.assertSynthOutput(
			{ Sweep.kr(Impulse.kr(ControlRate.ir/4), 1) * ControlRate.ir },
			[0, 1, 2, 3, 0, 1, 2, 3],
		);
		// Sweep: test start on 0
		this.assertSynthOutput({ Sweep.ar(DC.ar(0), 1) }, [0]); // ak
		this.assertSynthOutput({ Sweep.ar(DC.ar(0), DC.ar(1)) }, [0]); // aa
		this.assertSynthOutput({ Sweep.ar(DC.kr(0), 1) }, [0]); // kk
		this.assertSynthOutput({ Sweep.ar }, [0]); // 0k
		this.assertSynthOutput({ Sweep.ar(1) }, [0]); // 0k

		// Phasor: test start on start
		this.assertSynthOutput({ Phasor.ar(0, 1, 1, 10, 5) }, [1]);
		this.assertSynthOutput({ Phasor.kr(0, 1, 1, 10, 5) }, [1]);
		this.assertSynthOutput({ Phasor.ar(DC.ar(0), 1, 1, 10, 5) }, [1]);
		this.assertSynthOutput({ Phasor.ar(DC.ar(0), DC.ar(1), 1, 10, 5) }, [1]);
		// test start on reset when triggered at init
		this.assertSynthOutput({ Phasor.ar(1, 1, 0, 10, 5) }, [5]);
		// AK and AA subsample interpolation causes unexpected out: starts at 6
		this.assertSynthOutput({ Phasor.kr(1, 1, 0, 10, 5) }, [6]);
		this.assertSynthOutput({ Phasor.ar(DC.ar(1), 1, 0, 10, 5) }, [6]);
		this.assertSynthOutput({ Phasor.ar(DC.ar(1), DC.ar(1), 0, 10, 5) }, [6]);

		// MostChange: test initial choice: largest abs, b if abs(a) == abs(b)
		this.assertSynthOutput({ MostChange.ar([-1, 1], 0) }, [-1, 1]);
		this.assertSynthOutput({ MostChange.ar(0, [-1, 1]) }, [-1, 1]);
		this.assertSynthOutput({ MostChange.ar(1, -1) }, [-1]);
		// LeastChange: test initial choice: smallest abs, a if abs(a) == abs(b)
		this.assertSynthOutput({ LeastChange.ar([-1, 1], 0) }, [0, 0]);
		this.assertSynthOutput({ LeastChange.ar(0, [-1, 1]) }, [0, 0]);
		this.assertSynthOutput({ LeastChange.ar(1, -1) }, [1]);
	}
}
