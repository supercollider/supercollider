TestServer_nodeMove : UnitTest {

	var pipe;

	tearDown {
		if (pipe.notNil) {
			pipe.close;
		};
	}

	treeAfter { |program, moves|
		var order = List[],
		oscPath = PathName.tmp +/+ "dumpTree_test.osc",
		outPath = PathName.tmp +/+ "dummy_out.aiff",
		stepDur = 0.1,
		cmd, line, score, match, nodeTreeOutputDetected = false;

		score = Score();
		// build a known tree, then apply the moves and read back the resulting
		// depth-first node order (root id 0 dropped). /g_new args are:
		// new group id, add action (0 = head of target, 1 = tail of target), target id.
		// the result is two sibling groups under the root, each holding two child groups:
		//   1000 ( 2000 2001 )   1001 ( 3000 3001 )
		score.score = ([
			[0.0,
				['/g_new', 1000, 0, 0],
				['/g_new', 1001, 1, 0],
				['/g_new', 2000, 0, 1000],
				['/g_new', 2001, 1, 1000],
				['/g_new', 3000, 0, 1001],
				['/g_new', 3001, 1, 1001]
			],
		]);
		moves.do({ |m, i| score.add([stepDur * i, m])});
		score.add([stepDur * (1 + moves.size), ['/g_dumpTree', 0, 0]]); // dump after moves
		score.writeOSCFile(oscPath, 0, stepDur * (2 + moves.size));

		// construct the nrt command
		cmd = "% -N % _ % 44100 AIFF int16".format(program, oscPath.quote, outPath.quote);
		// run it and capture stdout
		pipe = Pipe.new(cmd, "r");
		line = pipe.getLine;
		while ({ line.notNil }) {
			// line.postln; // inspect output
			// ignore output until we see matching start
			if (line.beginsWith("NODE TREE")) {nodeTreeOutputDetected = true};
			// once we see matching beginning, capture output
			if (nodeTreeOutputDetected) {
				// line.postln; // inspect output
				match = line.findRegexp("^\\s+([0-9]+) group");
				if (match.notEmpty and: { match[1][1].asInteger != 0 }) {
					order.add(match[1][1].asInteger);
				};
			};
			// stop capturing when we see matching end
			if (line.beginsWith("END NODE")) {nodeTreeOutputDetected = false};
			line = pipe.getLine;
		};
		pipe.close;
		if (File.exists(oscPath)) { File.delete(oscPath)};
		if (File.exists(outPath)) { File.delete(outPath)};
		^order
	}

	// moving 2001 after 3000 should pull it out of group 1000 and place it
	// inside group 1001, right after 3000.
	test_moveAfter_acrossGroups {
		var moves = [["/n_after", 2001, 3000]];
		[Server.program, Server.program.replace("scsynth", "supernova")].do { |program|
			this.assertEquals(
				this.treeAfter(program, moves),
				List[1000, 2000, 1001, 3000, 2001, 3001],
				program.basename ++ " /n_after across groups should move the node into the target's group",
			);
		};
	}

	// first move 2000 into group 1001, then move it again within 1001. the second
	// move must resolve 2000's real (updated) parent, not a stale one, otherwise
	// supernova sends it back to group 1000.
	test_moveAfter_doesNotStaleParent {
		var moves = [["/n_after", 2000, 3000], ["/n_after", 2000, 3001]];
		[Server.program, Server.program.replace("scsynth", "supernova")].do { |program|
			this.assertEquals(
				this.treeAfter(program, moves),
				List[1000, 2001, 1001, 3000, 3001, 2000],
				program.basename ++ " a second /n_after should follow the node's real parent, not a stale one",
			);
		};
	}

}
