TestServer_dumpTree : UnitTest {

	var pid;

	setUp {
	}

	tearDown {
		pid !? {thisProcess.platform.killProcessByID(pid, true, true)};
	}

	getOutput { |program|
		var actualOutput = List[],
		oscPath = PathName.tmp +/+ "dumpTree_test.osc",
		outPath = PathName.tmp +/+ "dummy_out.aiff",
		logPath = PathName.tmp +/+ "nrt_output.log",
		cond = CondVar(),
		nodeTreeOutputDectected = false,
		cmd, line, score, logFile;

		score = Score();
		// this overwrites the automatically created default group, so that we only get the root group in the output
		score.score = ([
			[0.0, ['/g_dumpTree', 0, 0]],
			[0.1, [0]] // stop
		]);
		// write osc score to a file
		score.writeOSCFile(oscPath);
		// construct the nrt command
		cmd = "% -N % _ % 44100 AIFF int16 > % 2>&1".format(program, oscPath.quote, outPath.quote, logPath.quote);
		// protect against quote-stripping on Windows
		if(thisProcess.platform.name == \windows) {cmd = cmd.quote};
		// run command and capture output to a log file
		pid = cmd.unixCmd({pid = nil; cond.signalOne});
		cond.waitFor(60);
		// failsafe shutdown
		pid !? {
			"server process % (pid %) took too long, shutting down...".format(program.quote, pid).warn;
			thisProcess.platform.killProcessByID(pid, true, true)
		};
		// read output from the log file
		if(File.exists(logPath)) {
            logFile = File(logPath, "r");
			line = logFile.getLine;
			while ({ line.notNil }) {
				line.postln; // inspect output
				// ignore output until we see matching start
				if (line.beginsWith("NODE TREE")) {nodeTreeOutputDectected = true};
				// once we see matching beginning, capture output
				if (nodeTreeOutputDectected) {actualOutput.add(line)};
				// stop capturing when we see matching end
				if (line.beginsWith("END NODE")) {nodeTreeOutputDectected = false};
				line = logFile.getLine;
			};
			logFile.close;
		};
		// cleanup
		[oscPath, outPath, logPath].do({|path|
            if(File.exists(path)) {File.delete(path)};
		});
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
