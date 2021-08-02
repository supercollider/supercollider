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
		var firstSample;
		var cond = Condition.new;

		server.bootSync;

		{ PanAz.ar(2, DC.ar(1), -0.5) }.loadToFloatArray( 1 / server.sampleRate, server, { |output|
			firstSample = output[0];
			cond.unhang;
		});

		cond.hang;
		this.assertEquals(firstSample, 1.0, "Initial amplitude should match signals' amp");
	}

	test_PanAz_ar_initializesAmps {
		var firstSample;
		var cond = Condition.new;

		server.bootSync;

		{ PanAz.ar(2, DC.ar(1), DC.ar(-0.5)) }.loadToFloatArray( 1 / server.sampleRate, server, { |output|
			firstSample = output[0];
			cond.unhang;
		});

		cond.hang;
		this.assertEquals(firstSample, 1.0, "Initial amplitude should match signals' amp");
	}

}
