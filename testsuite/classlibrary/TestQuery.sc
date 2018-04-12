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

		this.assertEquals(responce, ['/b_info', buffer.bufnum, 512, 1, server.sampleRate], "Buffer.query should post correct info");

	}

	test_multiBufferQuery {

		var responces = Array(3);
		var buffers = 3.collect { Buffer.alloc(server, 512) };
		var bufnums;
		server.sync;

		bufnums = buffers.collect { |buf| buf.bufnum };
		buffers.do { |buf| buf.query({ |...args| responces.add(args[1]) }) };
		server.sync;

		this.assertEquals(responces, bufnums, "All buffers must be queried");

	}

}
