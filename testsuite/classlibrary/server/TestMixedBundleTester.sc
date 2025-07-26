TestMixedBundleTester : UnitTest {

	var server, tester;

	setUp {
		server = Server(this.class.name);
		server.bootSync;
		tester = MixedBundleTester.new;
		MixedBundleTester.reset;
	}

	tearDown {
		server.quit;
		server.remove;
	}

	test_findMessage {
		tester.add(["/n_free", 1001]);
		tester.send(server);

		server.latency.wait;
		0.01.wait;

		this.assert(MixedBundleTester.findMessage(["/n_free", 1001]), "should find the message in its sent messages");
		this.assert(MixedBundleTester.findMessage(["/n_free"]), "should match any /n_free message");
	}

	test_findPreparationMessage {
		var def;

		def = SynthDef("TestOSCBundle", { |out = 0, freq = 440|
			Out.ar(out, SinOsc.ar(freq));
		});

		// bad.
		// this is really testng UnitTest setUp
		// and is assuming the implementation of MixedBundle
		// this.assert( t.messages.isNil,"bundle should have no messages to start with");
		// this.assert( t.preparationMessages.isNil,"bundle should have no preparationMessages to start with");
		// anyway, they passed

		tester.addPrepare(["/d_recv", def.asBytes]);
		this.assertEquals(tester.preparationMessages.size, 1, "Bundle should have 1 preparationMessages");

		tester.send(server);
		(server.latency * 2).wait;

		this.assert(MixedBundleTester.findPreparationMessage(["/d_recv", def.asBytes]), "should find the synth def message in its preparation messages");
		this.assert(MixedBundleTester.findPreparationMessage(["/d_recv"]), "should match any synth def /d_recv");
	}

	// test that after sending that the bundle gets put in bundlesSent
	test_send {
		var sent;
		var numDefs = 100;
		var condvar = CondVar();

		this.makeDefs(numDefs);
		tester.addFunction({ condvar.signalOne });

		tester.send(server);
		condvar.waitFor(1);

		// should be 100 in preparationMessages
		this.assertEquals(MixedBundleTester.bundlesSent.size, 1, "should be 1 bundle sent");

		sent = MixedBundleTester.bundlesSent.first;
		this.assertEquals(sent, tester, "it should be our bundle");
		this.assertEquals(sent.preparationMessages.size, numDefs, "should be 100 in preparationMessages");
	}

	// crashes the language
	//	test_largePrepare {
	//		this.test_prepare(1000)
	//	}

	makeDefs { |numDefs|
		numDefs.do({ |i|
			var def = SynthDef("TestOSCBundle" ++ i, {
				Silent.ar
			});
			tester.addPrepare(["/d_recv", def.asBytes] )
		});
	}

	test_defNames {
		var def, names;
		def = [
			"/d_recv",
			Int8Array[83, 67, 103, 102, 0, 0, 0, 1, 0, 1, 7, 100, 105, 115, 107,
			73, 110, 52, 0, 5, 0, 0, 0, 0, 63, -128, 0, 0, 64, 0, 0, 0, 64, 64, 0, 0, -62,
			-58, 0, 0, 0, 7, 0, 0, 0, 0, 63, -128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			60, 35, -41, 10, 63, -128, 0, 0, 0, 7, 3, 111, 117, 116, 0, 0, 3, 97, 109, 112,
			0, 1, 6, 98, 117, 102, 110, 117, 109, 0, 2, 7, 115, 117, 115, 116, 97, 105, 110,
			0, 3, 2, 97, 114, 0, 4, 2, 100, 114, 0, 5, 4, 103, 97, 116, 101, 0, 6, 0, 20, 7,
			67, 111, 110, 116, 114, 111, 108, 1, 0, 0, 0, 7, 0, 0, 1, 1, 1, 1, 1, 1, 1, 6,
			68, 105, 115, 107, 73, 110, 2, 0, 2, 0, 4, 0, 0, 0, 0, 0, 2, -1, -1, 0, 0, 2, 2,
			2, 2, 5, 76, 105, 110, 101, 110, 1, 0, 5, 0, 1, 0, 0, 0, 0, 0, 6, 0, 0, 0, 4,
			-1, -1, 0, 1, 0, 0, 0, 5, -1, -1, 0, 2, 1, 12, 66, 105, 110, 97, 114, 121, 79,
			112, 85, 71, 101, 110, 2, 0, 2, 0, 1, 0, 2, 0, 1, 0, 0, 0, 2, 0, 0, 2, 12, 66,
			105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 2, 0, 2, 0, 1, 0, 2, 0, 1, 0,
			1, 0, 2, 0, 0, 2, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 2,
			0, 2, 0, 1, 0, 2, 0, 1, 0, 2, 0, 2, 0, 0, 2, 12, 66, 105, 110, 97, 114, 121, 79,
			112, 85, 71, 101, 110, 2, 0, 2, 0, 1, 0, 2, 0, 1, 0, 3, 0, 2, 0, 0, 2, 12, 66,
			105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1, 0, 2, 0, 1, 0, 1, 0, 0, 0,
			3, 0, 0, 0, 4, 1, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 1,
			0, 2, 0, 1, 0, 1, 0, 7, 0, 0, 0, 0, 0, 5, 1, 12, 66, 105, 110, 97, 114, 121, 79,
			112, 85, 71, 101, 110, 1, 0, 2, 0, 1, 0, 13, 0, 8, 0, 0, -1, -1, 0, 0, 1, 6, 69,
			110, 118, 71, 101, 110, 1, 0, 21, 0, 1, 0, 0, -1, -1, 0, 1, -1, -1, 0, 1, -1,
			-1, 0, 0, -1, -1, 0, 1, -1, -1, 0, 2, -1, -1, 0, 0, -1, -1, 0, 3, -1, -1, 0, 4,
			-1, -1, 0, 4, -1, -1, 0, 1, 0, 0, 0, 4, -1, -1, 0, 1, -1, -1, 0, 0, -1, -1, 0,
			1, 0, 9, 0, 0, -1, -1, 0, 1, -1, -1, 0, 0, -1, -1, 0, 0, 0, 0, 0, 5, -1, -1, 0,
			1, -1, -1, 0, 0, 1, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110,
			2, 0, 2, 0, 1, 0, 2, 0, 3, 0, 0, 0, 10, 0, 0, 2, 12, 66, 105, 110, 97, 114, 121,
			79, 112, 85, 71, 101, 110, 2, 0, 2, 0, 1, 0, 2, 0, 11, 0, 0, 0, 0, 0, 1, 2, 12,
			66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 2, 0, 2, 0, 1, 0, 2, 0,
			4, 0, 0, 0, 10, 0, 0, 2, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101,
			110, 2, 0, 2, 0, 1, 0, 2, 0, 13, 0, 0, 0, 0, 0, 1, 2, 12, 66, 105, 110, 97, 114,
			121, 79, 112, 85, 71, 101, 110, 2, 0, 2, 0, 1, 0, 2, 0, 5, 0, 0, 0, 10, 0, 0, 2,
			12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71, 101, 110, 2, 0, 2, 0, 1, 0, 2,
			0, 15, 0, 0, 0, 0, 0, 1, 2, 12, 66, 105, 110, 97, 114, 121, 79, 112, 85, 71,
			101, 110, 2, 0, 2, 0, 1, 0, 2, 0, 6, 0, 0, 0, 10, 0, 0, 2, 12, 66, 105, 110, 97,
			114, 121, 79, 112, 85, 71, 101, 110, 2, 0, 2, 0, 1, 0, 2, 0, 17, 0, 0, 0, 0, 0,
			1, 2, 3, 79, 117, 116, 2, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 14, 0,
			0, 0, 16, 0, 0, 0, 18, 0, 0, 0, 0]
		];

		tester.addPrepare(def);

		names = tester.defNames;

		this.assertEquals(names[0], "diskIn4");
		this.assert(tester.includesDefName("diskIn4"));
	}

}
