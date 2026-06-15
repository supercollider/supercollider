TestServer_dumpTree : UnitTest {

	var pipe;

	setUp {
	}

	tearDown {
		if (pipe.notNil) {
			pipe.close;
		};
	}

	getOutput { |program|
		var actualOutput = List[],
		oscPath = PathName.tmp +/+ "dumpTree_test.osc",
		outPath = PathName.tmp +/+ "dummy_out.aiff",
		cmd, line, score, nodeTreeOutputDectected = false;

		score = Score();
		// this overwrites the automatically created default group, so that we only get the root group in the output
		score.score = ([
			[0.0, ['/g_dumpTree', 0, 0]],
			[0.1, [0]] // stop
		]);
		// write osc score to a file
		score.writeOSCFile(oscPath);
		// construct the nrt command
		cmd = "% -N % _ % 44100 AIFF int16".format(program, oscPath.quote, outPath.quote);
		// run it and capture stdout
		pipe = Pipe.new(cmd, "r");
		line = pipe.getLine;
		while ({ line.notNil }) {
			// line.postln; // inspect output
			// ignore output until we see matching start
			if (line.beginsWith("NODE TREE")) {nodeTreeOutputDectected = true};
			// once we see matching beginning, capture output
			if (nodeTreeOutputDectected) {actualOutput.add(line)};
			// stop capturing when we see matching end
			if (line.beginsWith("END NODE")) {nodeTreeOutputDectected = false};
			line = pipe.getLine;
		};
		// cleanup
		pipe.close;
		if (File.exists(oscPath)) { File.delete(oscPath)};
		if (File.exists(outPath)) { File.delete(outPath)};

		^actualOutput;
	}

	test_dumpTree_scsynth {
		this.assertEquals(
			this.getOutput(Server.program),
			List[
				"NODE TREE Group 0",
				"END NODE TREE Group 0",
			],
			"scsynth /g_dumpTree output should match expected output",
		);
	}

	test_dumpTree_supernova {
		this.assertEquals(
			this.getOutput(Server.program.replace("scsynth", "supernova")),
			List[
				"NODE TREE Group 0",
				"   0 group",
				"END NODE TREE Group 0",
			],
			"supernova /g_dumpTree output should match expected output",
		);
	}

}
