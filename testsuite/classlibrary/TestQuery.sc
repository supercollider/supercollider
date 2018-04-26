TestQuery : UnitTest {

	var server;

	setUp {

		server = Server(this.class.name);
		this.bootServer(server);

	}

	tearDown {

		Buffer.freeAll;
		server.quit.remove;

	}

	test_bufferQuery {

		var response, buffer = Buffer.alloc(server, 512);
		server.sync;

		buffer.query({ |...args| response = args });
		server.sync;

		this.assertEquals(response, ['/b_info', buffer.bufnum, 512, 1, server.sampleRate], "Buffer.query should post correct info");

	}

	test_multiBufferQuery {

		var responses = Array(3);
		var buffers = 3.collect { Buffer.alloc(server, 512) };
		var bufnums;
		server.sync;

		bufnums = buffers.collect { |buf| buf.bufnum };
		buffers.do { |buf| buf.query({ |...args| responses.add(args[1]) }) };
		server.sync;

		this.assertEquals(responses, bufnums, "All requested buffers must be queried");

	}

	test_synthQuery {

		var response, synth = Synth(\default, target:server);
		server.sync;

		synth.query({ |...args| response = args });
		server.sync;

		this.assertEquals(response, ['/n_info', synth.nodeID, server.defaultGroupID, -1, -1, 0], "Synth.query should post correct info");

	}

	test_groupQuery {

		var response, group = Group(server);
		server.sync;

		group.query({ |...args| response = args });
		server.sync;

		this.assertEquals(response, ['/n_info', group.nodeID, server.defaultGroupID, -1, -1, 1, -1, -1], "Group.query should post correct info");

	}

	test_multiNodeQuery {

		var responses = Array(4);
		var nodes = 4.collect { |i| if (i%2 == 0) { Synth(\default, target:server) } { Group(server) } };
		var nodeIDs;
		server.sync;

		nodeIDs = nodes.collect { |node| node.nodeID };
		nodes.do { |node| node.query({ |...args| responses.add(args[1]) }) };
		server.sync;

		this.assertEquals(responses, nodeIDs, "All requested nodes must be queried");

	}

}
