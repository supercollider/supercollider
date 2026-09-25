TestSynthDefVersions_Server : UnitTest {
	var compiledDefs;

	setUp {
		var defArray1, defArray2, defArray3, stream, graphFunc, variants;

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

		defArray3 = [
			SynthDef(\test_v3_1, graphFunc, variants: variants),
			SynthDef(\test_v3_2, graphFunc, variants: variants)
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
		// write version 3
		stream = CollStream.on(Int8Array.new(256));
		defArray3.writeDef(stream, 3);
		compiledDefs = compiledDefs.add(3).add(stream.collection);
	}

	// helper method
	serverLoadsAllSynthDefVersions { arg serverName;
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
			server.quitSync;
		};

		server.remove;
	}

	test_scsynthLoadsAllSynthDefVersions {
		this.serverLoadsAllSynthDefVersions("scsynth");
	}

	test_supernovaLoadsAllSynthDefVersions {
		Server.supernova;
		this.serverLoadsAllSynthDefVersions("supernova");
		// restore scsynth
		Server.scsynth;
	}

	// helper method
	serverHandlesCorruptSynthDefs { arg serverName;
		var server = Server(thisMethod.name ++ "_" ++ serverName);

		this.bootServer(server);

		compiledDefs.pairsDo { |version, data|
			var stream, cond = Condition();
			data = data.copy; // !
			// remove the last 4 bytes
			data = data.extend(data.size - 4);
			// now try to read the corrupted SynthDef.
			// on every iteration, slice off half of the data.
			while { data.size > 1 } {
				server.sendMsg('/d_recv', data);
				data = data.extend(data.size div: 2);
			};
			// sync with timeout
			fork { 3.wait; cond.test_(true).signal };
			server.sync(cond);

			this.assert(server.serverRunning, "corrupted synthdef v% should not crash %".format(version, serverName));
		};

		if (server.serverRunning) {
			server.quitSync;
		};

		server.remove;
	}

	test_scsynthHandlesCorruptSynthDefs {
		this.serverHandlesCorruptSynthDefs("scsynth");
	}

	test_supernovaHandlesCorruptSynthDefs {
		Server.supernova;
		this.serverHandlesCorruptSynthDefs("supernova");
		// restore scsynth
		Server.scsynth;
	}
}
