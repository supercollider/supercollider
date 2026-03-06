TestServer_SynthDefVersions : UnitTest {
	var compiledDefs;

	setUp {
		var defArray1, defArray2, stream, graphFunc, variants;

		graphFunc = { |out, value=1| Out.kr(out, value) };
		variants = (a: [ value: 2 ], b: [ value: 3 ], c: [ value: 4 ]);

		// test with multiple SynthDefs per message. Also include variants.
		defArray1 = [
			SynthDefOld(\test_v1_1, graphFunc, variants: variants),
			SynthDefOld(\test_v1_2, graphFunc, variants: variants)
		];

		defArray2 = [
			SynthDef(\test_v2_1, graphFunc, variants: variants),
			SynthDef(\test_v2_2, graphFunc, variants: variants)
		];

		compiledDefs = [];
		// write version 1
		stream = CollStream.on(Int8Array.new(256));
		defArray1.writeDefOld(stream);
		compiledDefs = compiledDefs.add(1).add(stream.collection);
		// write version 2
		stream = CollStream.on(Int8Array.new(256));
		defArray2.writeDef(stream, 2);
		compiledDefs = compiledDefs.add(2).add(stream.collection);
	}

	// helper method
	serverLoadsOldSynthDefVersions { arg serverName;
		var server = Server(thisMethod.name ++ "_" ++ serverName);

		this.bootServer(server);

		compiledDefs.pairsDo { |version, bytes|
			var cond = Condition();
			// use a fresh pair of control busses for each def
			var ctlBus = Bus.control(server, 2);
			var defName;

			server.sendMsg('/d_recv', bytes);

			// sync with timeout
			fork { 3.wait; cond.test_(true).signal };
			server.sync(cond);

			this.assert(server.serverRunning, "synthdef v% should not crash %".format(version, serverName));

			defName = ("test_v"++version++"_1").asSymbol;
			Synth(defName, [ out: ctlBus.index, value: 5 ], server);

			defName = ("test_v"++version++"_2").asSymbol;
			Synth(defName, [ out: ctlBus.index + 1, value: 10 ], server);

			server.sync;

			cond.test = false;
			ctlBus.getn(2, { |array|
				var success = array[0] == 5 and: { array[1] == 10 };
				this.assert(success, "synthdef v% should load successfully".format(version));
				cond.test_(true).signal;
			});
			cond.wait;
		};

		if (server.serverRunning) {
			server.quit;
		};

		server.remove;
	}

	test_scsynthLoadsOldSynthDefVersions {
		this.serverLoadsOldSynthDefVersions("scsynth");
	}

	test_supernovaLoadsOldSynthDefVersions {
		Server.supernova;
		this.serverLoadsOldSynthDefVersions("supernova");
		// restore scsynth
		Server.scsynth;
	}
}
