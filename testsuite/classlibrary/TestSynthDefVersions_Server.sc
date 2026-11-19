TestSynthDefVersions_Server : UnitTest {
	var compiledDefs;

	setUp {
		var graphFunc = { |out, value=1| Out.kr(out, value) };
		var variants = (a: [ value: 2 ], b: [ value: 3 ], c: [ value: 4 ]);

		compiledDefs = (1..3).collect { |version|
			var stream, array;
			var defClass = [ SynthDefV1, SynthDefV2, SynthDef ][version-1];

			// test with multiple SynthDefs per message. Also include variants.
			array = [
				defClass.new(("test_v"++version++"_1").asSymbol, graphFunc, variants: variants),
				defClass.new(("test_v"++version++"_2").asSymbol, graphFunc, variants: variants)
			];

			stream = CollStream.on(Int8Array.new(256));
			array.writeDef(stream, version);

			version -> stream.collection;
		}
	}

	// helper method
	serverLoadsAllSynthDefVersions { arg serverName;
		var server = Server(thisMethod.name ++ "_" ++ serverName);

		compiledDefs.do { |item|
			var version = item.key, bytes = item.value;
			var timeout, ctlBus, success = false, cond = Condition();

			this.bootServer(server);

			server.sendMsg('/d_recv', bytes);

			// sync with timeout
			cond.test = false;
			timeout = fork { 3.wait; cond.test_(true).signal };
			server.sync(cond);
			timeout.stop;

			this.assert(server.serverRunning, "synthdef v% should not crash %".format(version, serverName));

			ctlBus = Bus.control(server, 2);

			Synth(("test_v"++version++"_1").asSymbol, [ out: ctlBus.index, value: 5 ], server);

			Synth(("test_v"++version++"_2").asSymbol, [ out: ctlBus.index + 1, value: 10 ], server);

			// sync again with timeout
			cond.test = false;
			timeout = fork { 3.wait; cond.test_(true).signal };
			server.sync(cond);
			timeout.stop;

			cond.test = false;
			ctlBus.getn(2, { |array|
				success = array[0] == 5 and: { array[1] == 10 };
				cond.test_(true).signal;
			});
			// wait with timeout
			timeout = fork { 3.wait; cond.test_(true).signal };
			cond.wait;
			timeout.stop;

			this.assert(success, "synthdef v% should load successfully".format(version));
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

		compiledDefs.do { |item|
			var version = item.key, data = item.value;
			var timeout, stream, cond = Condition();

			this.bootServer(server);

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
			cond.test = false;
			timeout = fork { 3.wait; cond.test_(true).signal };
			server.sync(cond);
			timeout.stop;

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
