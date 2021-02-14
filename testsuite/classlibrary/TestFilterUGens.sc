
TestFilterUGens : UnitTest {

	// check time invariance of all subClasses of Filter, particularly during the first 64 samples.
	// Filter followed by delay should be the same as delay followed by filter.
	// Only uses the filters' default arguments, but this should catch most initialisation errors.
	test_time_invariance {

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

		var testsIncomplete = delay_times.size * filters.size;

		this.bootServer;
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
				}.loadToFloatArray(0.1, Server.default, {
					arg data;
					this.assertArrayFloatEquals(data, 0, message, within:1e-10, report:true);
					testsIncomplete = testsIncomplete - 1;
				});

				rrand(0.012,0.035).wait;
			}
		};

		this.wait(testsIncomplete == 0);

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
		var cond = Condition.new;
		var server = Server.default;

		this.bootServer(server);
		server.sync;
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
		}.play;

		testResp = OSCFunc({ |msg|
			result = msg[3];
			cond.unhang;
		}, '/testIntegrator', server.addr, argTemplate: [synth.nodeID]);

		synth.onFree { cond.unhang };

		cond.hang;
		buffer.free;

		this.assertEquals(result, 1, "Integral of a single 1 should be 1");
	}

}
