TestUGen_Duty : UnitTest {

	var server;

	setUp {
		server = Server(this.class.name);
		this.bootServer(server);
	}

	tearDown {
		server.quit.remove;
	}

	/*
	func is called with dt (time between adjacent values) and the values as arguments.
	*/
	callAudioRateTestForFirstNFrames { |numFrames, func|
		var values;
		var dt, dur, action;
		var cond = Condition();

		values = [1, 0.15, 1.1, 0.25, 2, 0.5];

		dt = numFrames / server.sampleRate;
		dur = values.size * dt;
		action = { |ugenOutput|
			this.assertArrayFloatEquals(
				ugenOutput,
				values.stutter(numFrames),
				"Duty should output exact values at audio rate. Tested with % frames per value".format(numFrames)
			);
			cond.test_(true).signal;
		};
		{ func.value(dt, values) }.loadToFloatArray(dur, server, action);

		cond.wait;
	}

	test_ar_1_frame {
		this.callAudioRateTestForFirstNFrames(1, { |dt, values| Duty.ar(dt, 0, Dseq(values, 1)) })
	}

	test_ar_16_frames {
		this.callAudioRateTestForFirstNFrames(16, { |dt, values| Duty.ar(dt, 0, Dseq(values, 1)) })
	}

	test_ar_256_frames {
		this.callAudioRateTestForFirstNFrames(256, { |dt, values| Duty.ar(dt, 0, Dseq(values, 1)) })
	}

	test_ar_1_frame_audiorate_reset {
		this.callAudioRateTestForFirstNFrames(1, { |dt, values| Duty.ar(dt, DC.ar(0), Dseq(values, 1)) })
	}


	test_ar_1_frame_demandrate_reset {
		this.callAudioRateTestForFirstNFrames(1, { |dt, values| Duty.ar(dt, Dseq([inf], inf), Dseq(values, 1)) })
	}


}
