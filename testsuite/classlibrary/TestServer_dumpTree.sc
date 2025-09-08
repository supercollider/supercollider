TestServer_dumpTree : UnitTest {

	var addr, pipe;

	setUp {
	}

	tearDown {
		if (pipe.notNil) {
			pipe.close;
		};
	}

	test_dumpTree {
		var actualOutput, line;
		var programAndExpectedOutput = Dictionary.new;

		programAndExpectedOutput.put(
			"scsynth",
			List[
				"NODE TREE Group 0",
				"END NODE TREE Group 0",
			],
		);

		if(
			thisProcess.platform.name != "windows"
			and: { "which supernova".unixCmdGetStdOut.size > 0 }
		) {
			// supernova's /g_dumpTree output is different
			programAndExpectedOutput.put(
				"supernova",
				List[
					"NODE TREE Group 0",
					"   0 group",
					"END NODE TREE Group 0",
				]
			);
		};

		programAndExpectedOutput.keysValuesDo({ |program, expectedOutput|
			actualOutput = List[];
			pipe = Pipe.new(program ++ ServerOptions.new.asOptionsString, "r");
			// consume lines until the server is ready
			line = pipe.getLine;
			while ({ line.notNil && line.contains("ready").not }) {
				line = pipe.getLine;
			};
			// connect OSC, dump the tree, quit the server
			addr = NetAddr("127.0.0.1", 57110);
			addr.sendMsg("/g_dumpTree", 0, 0);
			addr.sendMsg("/quit");
			// consume lines until no more remain
			line = pipe.getLine;
			while ({ line.notNil }) {
				actualOutput.add(line);
				line = pipe.getLine;
			};
			pipe.close;
			this.assertEquals(
				actualOutput,
				expectedOutput,
				program ++ " /g_dumpTree output should match expected",
			);
		});
	}

}
