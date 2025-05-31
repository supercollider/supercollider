TestPanAz : UnitTest {

	var server;

	setUp {
		server = Server(this.class.name);
	}

	tearDown {
		if(server.serverRunning) { server.quitSync };
		server.remove;
	}

	test_PanAz_ak_initializesAmps {
		var firstSample = nil;
		var cond = CondVar();

		server.bootSync;

		{
			PanAz.ar(2, DC.ar(1), -0.5)
		}.loadToFloatArray(1 / server.sampleRate, server) { |output|
			firstSample = output[0];
			cond.signalOne;
		};

		if(cond.waitFor(0.5, {firstSample.notNil})) {
			this.assertFloatEquals(firstSample, 1.0, "Initial amplitude should match signals' amp");
		} {
			this.failed(currentMethod, "timeout while waiting for synth output")
		};

	}

	test_PanAz_ar_initializesAmps {
		var firstSample = nil;
		var cond = CondVar();

		server.bootSync;

		{
			PanAz.ar(2, DC.ar(1), DC.ar(-0.5))
		}.loadToFloatArray(1 / server.sampleRate, server) { |output|
			firstSample = output[0];
			cond.signalOne;
		};

		if(cond.waitFor(0.5, {firstSample.notNil})) {
			this.assertFloatEquals(firstSample, 1.0, "Initial amplitude should match signals' amp");
		} {
			this.failed(currentMethod, "timeout while waiting for synth output")
		};
	}

	test_PanAz_crosstalk {
		var passed;
		var cond = Condition.new;
		var expected = [0.0, 1.0, 0.0, 0.0];

		server.bootSync;

		{ PanAz.ar(4, DC.ar(1), DC.ar(0.25), orientation: 0.5) }
		.loadToFloatArray( 1 / server.sampleRate, server, { |output|
			passed = output.detect{|v,n| expected[n] != v}.isNil;
			cond.unhang;
		});

		cond.hang;

		this.assert(passed, "Panning to one channel should not result in crosstalk");
	}

	test_PanAz_equalPowerBetweenTwoAdjacentChannels {
		var result;
		var cond = Condition.new;

		server.bootSync;

		{ PanAz.ar(4, DC.ar(1), DC.ar(0), orientation: 0.5) }
		.loadToFloatArray( 1 / server.sampleRate, server, { |output|
			result = output;
			cond.unhang;
		});

		cond.hang;

		this.assertEquals(result[1],result[0], "Panning halfways between two channels should produce same power in both channels");
	}
}
