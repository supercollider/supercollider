TestPanAz : UnitTest {

	var server;

	setUp {
		server = Server(this.class.name);
	}

	tearDown {
		if(server.serverRunning) { server.quit };
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

}
