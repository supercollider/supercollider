TestBuffer_Server : UnitTest {

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

	test_freeAll {
		var allocator = server.bufferAllocator;
		var initNumBufs = allocator.blocks.size;

		Buffer(server, 512);
		server.sync;
		this.assertEquals(allocator.blocks.size, (initNumBufs + 1), "There should be 1 additional buffer allocated");

		Buffer.freeAll(server);
		server.sync;
		this.assertEquals(allocator.blocks.size, 0, "There should no longer be any buffers allocated after Buffer.freeAll");
	}

	test_serverlang_dataexchange {
		var data = Array.fill(512, { 1.0.rand });
		var buffer;

		// send then load
		buffer = Buffer.sendCollection(server, data);
		server.sync;
		buffer.loadToFloatArray(action: { |array|
			this.assertArrayFloatEquals(array, data, "sendCollection->loadToFloatArray gave back similar data")
		});
		server.sync;
		buffer.free;

		// load then load
		buffer = Buffer.loadCollection(server, data);
		server.sync;
		buffer.loadToFloatArray(action: { |array|
			this.assertArrayFloatEquals(array, data, "loadCollection->loadToFloatArray gave back similar data")
		});
		server.sync;

		// zero the Buffer and check that it worked
		buffer.zero;
		server.sync;
		buffer.loadToFloatArray(action: { |array|
			this.assertArrayFloatEquals(array, 0, "Buffer:zero should give a buffer containing zeroes")
		});
		server.sync;
	}


	// Note that the "expected values" used in this test depend precisely on the samples in a11wlk01.wav, so will need updating if that changes.
	test_allocAndQuery {
		var timeout, condition = Condition.new;
		var buffer_number = 9, num_frames = 512, num_channels = 1, buffer_sampleRate = 44100;
		var frame_value = -0.001617431640625;
		var query, get;
		var query_reply = ['/b_info', buffer_number, num_frames, num_channels, buffer_sampleRate];
		var get_reply = ['/b_set', buffer_number, (num_frames / 2), frame_value];

		OSCFunc({ |msg| query = msg; condition.unhang }, '/b_info', server.addr).oneShot;
		OSCFunc({ |msg| get = msg; condition.unhang }, '/b_set', server.addr).oneShot;

		server.sendMsg(
			'/b_allocRead',
			buffer_number,
			Platform.resourceDir +/+ "sounds/a11wlk01.wav",
			0, // starting frame
			num_frames
		);
		server.sync;

		timeout = fork { 3.wait; condition.unhang };
		server.sendMsg('/b_query', buffer_number);
		condition.hang;
		timeout.stop.reset;

		this.assertEquals(query, query_reply, "/b_info data returned from a /b_query message should be as expected");

		timeout.play;
		server.sendMsg('/b_get', buffer_number, (num_frames / 2));
		condition.hang;
		timeout.stop;

		this.assertEquals(get, get_reply, "/b_set data returned from a /b_get message should be as expected");
	}

	test_cheby {
		var timeout, condition = Condition.new;
		var buffer, size = 512;
		var fromBuffer;
		var calcVal;

		calcVal = cos(2 * acos((size - 1) / (size >> 1) - 1)) + 1;
		buffer = Buffer.alloc(server, size, completionMessage: { |buf| buf.chebyMsg([0, 1], normalize: false, asWavetable: false) });
		server.sync;

		buffer.loadToFloatArray(action: { |array|
			fromBuffer = array;
			condition.unhang;
		});

		timeout = fork { 1.wait; condition.unhang };
		condition.hang;
		timeout.stop.reset;

		this.assert(
			fromBuffer.first.equalWithPrecision(2) &&
			this.getSignalMidValue(fromBuffer).equalWithPrecision(0) &&
			fromBuffer.last.equalWithPrecision(calcVal),
			"Order 2 Cheby Buffer (zero-offset) should be a parabola between 0 and 2"
		);

		calcVal = [size >> 1, size-1].linlin(0, size, -1, 1);
		buffer.cheby([1], asWavetable: false);
		server.sync;

		buffer.loadToFloatArray(action: { |array|
			fromBuffer = array;
			condition.unhang;
		});

		timeout.play;
		condition.hang;
		timeout.stop.reset;

		this.assert(
			fromBuffer.first.equalWithPrecision(-1) &&
			this.getSignalMidValue(fromBuffer).equalWithPrecision(calcVal.first) &&
			fromBuffer.last.equalWithPrecision(calcVal.last),
			"Order 1 Cheby Buffer should go from -1 to % by default".format(calcVal.last)
		);

		calcVal = cos(2 * acos((size - 1)/(size >> 1) - 1)) + 1 / 2;
		buffer.cheby([0, 1], normalize: true, asWavetable: false);
		server.sync;

		buffer.loadToFloatArray(action: { |array|
			fromBuffer = array;
			condition.unhang;
		});

		timeout.play;
		condition.hang;
		timeout.stop.reset;

		this.assert(
			fromBuffer.first.equalWithPrecision(1) &&
			this.getSignalMidValue(fromBuffer).equalWithPrecision(0) &&
			fromBuffer.last.equalWithPrecision(calcVal),
			"Order 2 Cheby Buffer should be a parabola between from 0 to 1 by default (normalized, zero-offset)"
		);

		buffer.cheby([0, 1, 0.5, -0.25], normalize: false, asWavetable: false);
		server.sync;

		buffer.loadToFloatArray(action: { |array|
			fromBuffer = array;
			condition.unhang;
		});

		timeout.play;
		condition.hang;
		timeout.stop;

		this.assert(
			this.getSignalMidValue(fromBuffer).equalWithPrecision(0),
			"A zeroOffset Cheby Buffer should have value 0 for its middle value"
		);
	}

	// This is just a helper method
	getSignalMidValue { |sig|
		^sig[sig.size >> 1]
	}

	test_get {
		var timeout, condition = Condition.new;
		var buffer, collection;
		var get_value;

		collection = [88.88, 8, 888.8, 8.88];
		buffer = Buffer.sendCollection(server, collection);
		server.sync;

		buffer.get(2, { |value|
			get_value = value;
		});

		timeout = fork { 3.wait; condition.unhang };
		condition.hang;
		timeout.stop;

		this.assertFloatEquals(get_value, collection[2], "Buffer:get should get the requested value from the buffer");
	}

	test_getn {
		var timeout, condition = Condition.new;
		var buffer, collection;
		var getn_values;

		collection = [88.88, 8, 888.8, 8.88];
		buffer = Buffer.sendCollection(server, collection);
		server.sync;

		buffer.getn(2, 2, { |values|
			getn_values = values;
		});

		timeout = fork { 3.wait; condition.unhang };
		condition.hang;
		timeout.stop;

		this.assertArrayFloatEquals(getn_values, collection[2..3], "Buffer:getn should get the requested values from the buffer");
	}

	test_regression_getn_float {
		var timeout, condition = Condition.new;
		var buffer, collection;
		var getn_values;

		collection = [88.88, 8, 888.8, 8.88];
		buffer = Buffer.sendCollection(server, collection);
		server.sync;

		// Using a float as the index should still work
		buffer.getn(2.0, 2, { |values|
			getn_values = values;
		});

		timeout = fork { 3.wait; condition.unhang };
		condition.hang;
		timeout.stop;

		this.assertArrayFloatEquals(getn_values, collection[2..3], "Buffer:getn should get the requested values from the buffer");
	}

	test_getToFloatArray {
		var timeout, condition = Condition.new;
		var buffer, collection;
		var floats;

		collection = [88.88, 8, 888.8, 8.88];
		buffer = Buffer.sendCollection(server, collection);
		server.sync;

		buffer.getToFloatArray(action: { |array|
			floats = array;
			condition.unhang;
		});

		timeout = fork { 3.wait; condition.unhang };
		condition.hang;
		timeout.stop;

		this.assertArrayFloatEquals(floats, collection, "Buffer:getToFloatArray should get the buffer's values");
	}

	test_getToFloatArray_index_count {
		var collection = Array.iota(8).asFloat;
		var buffer = Buffer.sendCollection(server, collection);
		var ranges = [
			[0, nil],
			[0, -1],
			[2, nil],
			[2, -1],
			[2, 2],
		];

		ranges.do { |range|
			var floats;
			var slice = if(range[1] == -1 || range[1].isNil) {
				collection.copyToEnd(range[0]);
			} {
				collection.copyRange(range[0], (range[0] + range[1]) - 1);
			};

			buffer.getToFloatArray(
				index: range[0],
				count: range[1],
				action: { |array| floats = array }
			);
			server.sync;

			this.assertArrayFloatEquals(
				floats,
				slice,
				"getToFloatArray should get count number of values starting at index"
			);
		};
	}

}
