TestQuery : UnitTest {

	var server;

	setUp {

		server = Server.default;
		this.bootServer(server);

	}

	tearDown {

		Buffer.freeAll;
		server.quit;

	}

	test_bufferQuery {

		var responce, buffer = Buffer.alloc(server, 512);
		server.sync;

		buffer.query({ |...args| responce = args });
		server.sync;

		this.assertEquals(responce, ['/b_info', 0, 512, 1, server.sampleRate], "Buffer.query should post correct info");

	}

	test_multiBufferQuery {

		var bufnums = Array(3);
		var buffers = 3.collect { Buffer.alloc(server, 512) };
		server.sync;

		buffers.do { |buf| buf.query({ |...args| bufnums.add(args[1]) }) };
		server.sync;

		this.assertEquals(bufnums, [0, 1, 2], "All buffers must be queried");

	}

}
