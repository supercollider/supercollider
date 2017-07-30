TestUGen_Duty : UnitTest {

	/*
	func is called with dt and values.
	*/

	callAudioRateTestForFirstNFrames { |n, func|
		var values;
		var server = Server.default;
		var dt, dur, action;

		values = [1, 0.15, 1.1, 0.25, 2, 0.5];

		dt = n / server.sampleRate;
		dur = values.size * dt;
		action = { |ugenOutput|
			this.assertArrayFloatEquals(
				ugenOutput,
				values.stutter(n),
				"Duty should output exact values at audio rate. Tested with % frames per value".format(n)
			);
		};
		{ func.value(dt, values) }.loadToFloatArray(dur, server, action);
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
