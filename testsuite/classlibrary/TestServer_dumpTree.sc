TestServer_dumpTree : UnitTest {

    var pipe;

    setUp {
    }

    tearDown {
        if (pipe.notNil) {
            pipe.close;
        };
    }

    getOutput { |program, expectedOutput|
        var actualOutput = List[],
            addr = NetAddr("127.0.0.1", 57110),
            line;
        pipe = Pipe.new(program ++ ServerOptions.new.asOptionsString, "r");
        // consume lines until the server is ready
        line = pipe.getLine;
        while ({ line.notNil && line.contains("ready").not }) {
            line = pipe.getLine;
        };
        // dump the tree, quit the server
        addr.sendMsg("/g_dumpTree", 0, 0);
        addr.sendMsg("/quit");
        // consume lines until no more remain
        line = pipe.getLine;
        while ({ line.notNil }) {
            // This warning can appear in GitHub Actions runs for supernova
            if (
                line != "Warning: cannot raise thread priority",
                { actualOutput.add(line); }
            );
            line = pipe.getLine;
        };
        pipe.close;
        ^ actualOutput;
    }	

    test_dumpTree_scsynth {
        this.assertEquals(
            this.getOutput(Server.program),
            List[
                "NODE TREE Group 0",
                "END NODE TREE Group 0",
            ],
            "scsynth /g_dumpTree output should match expected",
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
            "supernova /g_dumpTree output should match expected",
        );
    }

}
