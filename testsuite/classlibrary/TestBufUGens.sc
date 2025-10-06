TestBufUGens : UnitTest {

	var server;

	setUp {
		server = Server(this.class.name);
		server.bootSync;
	}

	tearDown {
		Buffer.freeAll(server);
		server.quit;
		server.remove;
	}

	test_BufIn_readCorrectValues {
		var data = [89, -2.0, 12345678, -0.1];
		var buffer = Buffer.sendCollection(server, data);
		var period = server.options.blockSize / server.sampleRate;
		var cond = CondVar();
		var out = nil;
		server.sync;
		{ BufIn.kr(data.size, buffer) }.loadToFloatArray(period, server) { |v|
				out = v;
				cond.signalOne;
		};
		cond.waitFor(1){ out.notNil };

		this.assertArrayFloatEquals(out, data, "should output correct buffer values", 1e-5);

		buffer.free;
	}

}
