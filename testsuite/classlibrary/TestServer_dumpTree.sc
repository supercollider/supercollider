TestServer_dumpTree : UnitTest {

    var pipe;

	*features { ^[UnitTestFeatures.needsServerBoot] }

    setUp {
    }

    tearDown {
        if (pipe.notNil) {
            pipe.close;
        };
    }

    getOutput { |program, expectedOutput|
        var actualOutput = List[],
            ip = "127.0.0.1",
            port = 57110,
            addr = NetAddr(ip, port),
            line,
			count = 0;
        pipe = Pipe.new(program ++ ServerOptions().bindAddress_(ip).asOptionsString(port), "r");
        // consume lines until the server is ready
        line = pipe.getLine;
        while ({ line.notNil && line.contains("ready").not }) {
            line = pipe.getLine;
			count = count + 1;
			if (count > 1000) { Error("Too much iteration").throw }
        };
		count = 0;
        // dump the tree, quit the server
        addr.sendMsg("/g_dumpTree", 0, 0);
        addr.sendMsg("/quit");
        // consume lines until no more remain
        line = pipe.getLine;
        while ({ line.notNil }) {
            // Ignore transient unrelated warnings
            if (
                line.beginsWith("Warning:").not,
                { actualOutput.add(line); }
            );
            line = pipe.getLine;

			count = count + 1;
			if (count > 1000) { Error("Too much iteration").throw }
        };
        pipe.close;
        ^actualOutput;
    }	

    test_dumpTree_scsynth {
        this.assertEquals(
            this.getOutput(Server.scsynthProgram),
            List[
                "NODE TREE Group 0",
                "END NODE TREE Group 0",
            ],
            "scsynth /g_dumpTree output should match expected",
        );
    }

    test_dumpTree_supernova {
        this.assertEquals(
            this.getOutput(Server.supernovaProgram),
            List[
                "NODE TREE Group 0",
                "   0 group",
                "END NODE TREE Group 0",
            ],
            "supernova /g_dumpTree output should match expected",
        );
    }

}
