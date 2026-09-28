TestServer_nodeOrder : UnitTest {

	var pipe;

	tearDown {
		if (pipe.notNil) {
			pipe.close;
		};
	}

	orderAfter { |program, action, target, nodes|
		var order = List[],
			ip = "127.0.0.1",
			port = 57110,
			addr = NetAddr(ip, port),
			line,
			match;
		pipe = Pipe.new(program ++ ServerOptions().bindAddress_(ip).asOptionsString(port), "r");
		line = pipe.getLine;
		while ({ line.notNil && line.contains("ready").not }) {
			line = pipe.getLine;
		};
		addr.sendMsg("/g_new", 1000, 0, 0);
		addr.sendMsg("/g_new", 1001, 3, 1000);
		addr.sendMsg("/g_new", 1002, 3, 1001);
		addr.sendMsg("/n_order", action, target, *nodes);
		addr.sendMsg("/g_dumpTree", 0, 0);
		addr.sendMsg("/quit");
		line = pipe.getLine;
		while ({ line.notNil }) {
			match = line.findRegexp("^\\s+([0-9]+) group");
			if (match.notEmpty and: { match[1][1].asInteger != 0 }) {
				order.add(match[1][1].asInteger);
			};
			line = pipe.getLine;
		};
		pipe.close;
		^ order
	}

	test_nOrder_head {
		[Server.program, Server.program.replace("scsynth", "supernova")].do { |program|
			this.assertEquals(
				this.orderAfter(program, 0, 0, [1002, 1001, 1000]),
				List[1002, 1001, 1000],
				program.basename ++ " /n_order head should apply the listed order at the group head",
			);
		};
	}

	test_nOrder_after {
		[Server.program, Server.program.replace("scsynth", "supernova")].do { |program|
			this.assertEquals(
				this.orderAfter(program, 3, 1000, [1002, 1001]),
				List[1000, 1002, 1001],
				program.basename ++ " /n_order after should chain the listed order behind the target",
			);
		};
	}

}
