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

  assertBufInOuptut { |data, message, numSamples(data.size), channel=0, offset=0, expected(data), timeout=1|
		var cond = CondVar();
		var out = nil;
		var buffer = if (data.rank > 1) {
				Buffer.sendCollection(server, data.lace, data.shape[0])
		} {
				Buffer.sendCollection(server, data, 1)
		};
		server.sync;
		[\ar, \kr].do { |method|
				var period = if (method === \ar) { 1 } { server.options.blockSize } / server.sampleRate;
				{ BufIn.perform(method, numSamples, buffer, channel, offset) }
				.loadToFloatArray(period, server) { |v|
						out = v;
						cond.signalOne;
				};
				cond.waitFor(1){ out.notNil };

				if (out.isNil) {
						this.assert(false,
								"timeout while expecting values from server")
				} {
						this.assertArrayFloatEquals(out, expected, message + " (%)".format(method), 1e-5)
				};
		};

		server.sync; // make sure synths are freed before freeing buffer
		buffer.free;
		server.sync; // make sure buf is freed before moving on
  }

	test_BufIn_readCorrectValues {
		var data = [89, -2.0, 12345678, -0.1];
		this.assertBufInOuptut(data,  "should output correct buffer values");

		data = [[19, -4.0], [1234678, -0.5]].lace;
		this.assertBufInOuptut(data,
				"should output interleaved buffer values for a multi-channel buffer",
				channel: -1,
				expected: data.lace
		);

		data = [[19, -4.0], [1234678, -0.5]];
		this.assertBufInOuptut(data,
				"should read correct channel of a multi-channel buffer",
				channel: 1,
				numSamples: data[1].size,
				expected: data[1]
		);

		this.assertBufInOuptut(data,
				"should output zeros when reading an exceeding channel of a multi-channel buffer",
				channel: 10,
				expected: data.lace.collect(0)
		);

		// TODO: refactor assertBufInOuptut for this to make sense
		// data = [[19, -4.0], [1234678, -0.5]];
		// this.assertBufInOuptut(data,
		// 		"should default to reading channel 0 of a multi-channel buffer",
		// 		expected: data[0],
		// );

		// TODO: test for multi-channel expansion of channels?

	}

	test_BufIn_readCorrectValues_numOutputs {
		var data = [123, -456];
		this.assertBufInOuptut(data,
				"should output correct buffer values followed by zeros for extra channels",
				numSamples: 4,
				expected: [123, -456, 0, 0]
		);

		data = [456, -789, 123, -456];
		this.assertBufInOuptut(data,
				"should output correct buffer values when reading less channels",
				numSamples: 2,
				expected: [456, -789]
		);
	}

	test_BufIn_readCorrectValues_offsets {
		var data = [56, -12, 0.5, 1];
		this.assertBufInOuptut(data,
				"should output correct buffer values when given a valid offset",
				numSamples: 2, offset: 2,
				expected: [0.5, 1]
		);

		data = [456, -123, 891];
		this.assertBufInOuptut(data,
				"should start from 0 when given offset is negative",
				offset: -2,
		);

		data = [4567, -3123, 817];
		this.assertBufInOuptut(data,
				"should output zeros when given offset is too big",
				offset: 4,
				expected: [0, 0, 0]
		);

		data = [46, -23];
		this.assertBufInOuptut(data,
				"should output correct buffer values when given a float offset (truncated)",
				offset: 0.95,
				expected: [46, -23]
		);
	}
}
