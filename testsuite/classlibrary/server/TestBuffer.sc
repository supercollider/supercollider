/*
Buffer.test
UnitTest.gui
*/
TestBuffer : UnitTest {

	test_freeAll {
		var n;

		this.bootServer;

		Buffer.freeAll;
		n = Buffer(Server.default,44100,2);
		this.assertEquals( Server.default.bufferAllocator.blocks.size , 1 , " allocated one buffer");
		Buffer.freeAll;
		this.assertEquals( Server.default.bufferAllocator.blocks.size , 0 , "freeAll : no buffers allocated");

	}

	test_serverlang_dataexchange {
		var d, b;

		this.bootServer;

		// Random data for test
		d = {1.0.rand}.dup(Server.default.sampleRate);

		// send then load
		b = Buffer.sendCollection(Server.default, d);
		0.2.wait;
		Server.default.sync;
		b.loadToFloatArray(0, -1, action:{|data|
			this.assert(data.size==d.size,       "test_serverlang_dataexchange sendCollection->loadToFloatArray received full-length data (%==%)".format(data.size, d.size));
			this.assertArrayFloatEquals(data, d, "test_serverlang_dataexchange sendCollection->loadToFloatArray", report: true)
		});
		Server.default.sync;
		b.free;
		Server.default.sync;

		// load then load
		b = Buffer.loadCollection(Server.default, d);
		0.2.wait;
		Server.default.sync;
		b.loadToFloatArray(0, -1, action:{|data|
			this.assert(data.size==d.size,       "test_serverlang_dataexchange loadCollection->loadToFloatArray received full-length data (%==%)".format(data.size, d.size));
			this.assertArrayFloatEquals(data, d, "test_serverlang_dataexchange loadCollection->loadToFloatArray", report: true)
		});
		Server.default.sync;

		// zero the Buffer and check that it worked
		b.zero;
		Server.default.sync;
		b.loadToFloatArray(0, -1, action:{|data|
			this.assertArrayFloatEquals(data, 0, "Buffer:zero should give a buffer containing zeroes", report: true)
		});
		Server.default.sync;
		b.free;
		Server.default.sync;
	}


	// Test basen on JP's test code, for the bug which DS introduced in svn rev 9362, and JP fixed in svn rev 9371
	// Note that the "expected values" used in this test depend precisely on the samples in a11wlk01.wav, so will need updating if that changes.
	test_allocAndQuery {
		var expectq, expectg, s=this.s, sfpath, sf, b;
		this.bootServer;
		Buffer.freeAll;

		OSCFunc({ |message|
			this.assertEquals(message, expectq, "/b_info data returned from a /b_query message should be as expected");
		}, \b_info, s.addr).oneShot;

		OSCFunc({ |message|
			this.assertEquals(message, expectg, "/b_set data returned from a /b_get message should be as expected");
		}, \b_set, s.addr).oneShot;

		// Hmm, seems not all systems have the standard soundfile available
		// s.sendMsg(\b_allocRead, 1, "sounds/a11wlk01.wav");
		// So instead we use this from the SoundFile helpfile:
		sf = SoundFile.new.headerFormat_("AIFF").sampleFormat_("int16").numChannels_(1);
		sfpath = PathName.tmp +/+ "sc3_test_allocAndQuery.aiff";
		sf.openWrite(sfpath);
		// sawtooth
		b = Signal.sineFill(441, (1..20).reciprocal);
		// write multiple cycles (441 * 100 = 1 sec worth)
		100.do{ sf.writeData(b) };
		sf.close;
		0.1.wait;
		s.sendMsg(\b_allocRead, 1, sfpath);

		0.4.wait;
		s.sync;

		expectq = [ '/b_info', 1, 44100, 1, 44100 ];
		expectg = [ '/b_set', 1, 1000, 0.43011474609375 ];
		s.sendMsg(\b_query, 1);
		s.sendMsg(\b_get, 1, 1000);

		0.4.wait;
		s.sync;

		s.sendMsg(\b_alloc, 3, 30000);
		s.sendMsg(\b_read, 3, sfpath, 0, 30000);

		0.4.wait;
		s.sync;

		expectq = [ '/b_info', 3, 30000, 1, 44100 ];
		expectg = [ '/b_set', 3, 1000, 0.43011474609375 ];
		s.sendMsg(\b_query, 3);
		s.sendMsg(\b_get, 3, 1000);

		0.4.wait;
		s.sync;


	}

	test_cheby {
		var s = this.s;
		var buf;
		var fromBuffer;
		var calcVal;
		var bufSize = 512;

		this.bootServer;
		buf = Buffer.alloc(s, bufSize , 1, { arg b; b.chebyMsg([0, 1], normalize: false, asWavetable: false) });
		s.sync;
		buf.loadToFloatArray(action: { arg array; fromBuffer = array });

		calcVal = cos(2 * acos((bufSize - 1)/(bufSize >> 1) - 1)) + 1;
		this.assert(
			fromBuffer.first.equalWithPrecision(2) &&
			this.getSignalMidValue(fromBuffer).equalWithPrecision(0) &&
			fromBuffer.last.equalWithPrecision(calcVal),
			"Order 2 Cheby Buffer (zero-offset) should be a parabola between 0 and 2");

		buf.cheby([1], asWavetable: false);
		buf.loadToFloatArray(action: { arg array; fromBuffer = array });
		calcVal = [bufSize >> 1, bufSize-1].linlin(0, bufSize, -1, 1);
		this.assert(
			fromBuffer.first.equalWithPrecision(-1) &&
			this.getSignalMidValue(fromBuffer).equalWithPrecision(calcVal.first) &&
			fromBuffer.last.equalWithPrecision(calcVal.last),
			"Order 1 Cheby Buffer should go from -1 to % by default".format(calcVal.last));

		buf.cheby([0, 1], normalize: true, asWavetable: false);
		buf.loadToFloatArray(action: { arg array; fromBuffer = array });
		calcVal = cos(2 * acos((bufSize - 1)/(bufSize >> 1) - 1)) + 1 / 2;
		this.assert(
			fromBuffer.first.equalWithPrecision(1) &&
			this.getSignalMidValue(fromBuffer).equalWithPrecision(0) &&
			fromBuffer.last.equalWithPrecision(calcVal),
			"Order 2 Cheby Buffer should be a parabola between from 0 to 1 by default (normalized, zero-offset)");

		buf.cheby([0, 1, 0.5, -0.25], normalize: false, asWavetable: false);
		buf.loadToFloatArray(action: { arg array; fromBuffer = array });
		this.assert(
			this.getSignalMidValue(fromBuffer).equalWithPrecision(0),
			"A zeroOffset Cheby Buffer should have value 0 for its middle value");

	}

	// This is just a helper method
	getSignalMidValue { arg sig;
		^sig[sig.size >> 1]
	}

	test_get {
		var s, buffer, collection;
		var get_value;
		s = this.s;
		this.bootServer;

		collection = [88.88, 8, 888.8, 8.88];
		buffer = Buffer.sendCollection(s, collection);
		s.sync;

		get_value = 0;
		buffer.get(2, { |value|
			get_value = value;
		});

		0.1.wait;

		// Some precision is lost when converting to 32-bit float, hence the use of
		// assertFloatEquals.
		this.assertFloatEquals(get_value, collection[2], "Buffer:get works", 0.001);

		buffer.free;
	}

	test_getn {
		var s, buffer, collection;
		var getn_values;
		s = this.s;
		this.bootServer;

		collection = [88.88, 8, 888.8, 8.88];
		buffer = Buffer.sendCollection(s, collection);
		s.sync;

		getn_values = [0, 0];
		buffer.getn(2, 2, { |values|
			getn_values = values;
		});

		0.1.wait;

		// See comments in test_get.
		this.assertArrayFloatEquals(getn_values, collection[2..3], "Buffer:getn works", 0.001);

		buffer.free;
	}

}
