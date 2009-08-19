
TestBuffer : UnitTest {
	
	test_freeAll {
		var n;
		Buffer.freeAll;
		n = Buffer(Server.default,44100,2);
		this.assertEquals( Server.default.bufferAllocator.blocks.size , 1 , " allocated one buffer");
		Buffer.freeAll;
		this.assertEquals( Server.default.bufferAllocator.blocks.size , 0 , "freeAll : no buffers allocated");
		
	}
	
	test_serverlang_dataexchange {
		var d, b;
		
		this.bootServer;
		
		// Random data for test
		d = {1.0.rand}.dup(Server.default.sampleRate);
		
		// send then load
		b = Buffer.sendCollection(Server.default, d);
		Server.default.sync;
		b.loadToFloatArray(0, -1, action:{|data| 
			this.assertArrayFloatEquals(data - d, 0, report: true)
		});
		// load then load
		b = Buffer.loadCollection(Server.default, d);
		Server.default.sync;
		b.loadToFloatArray(0, -1, action:{|data| 
			this.assertArrayFloatEquals(data - d, 0, report: true)
		});
		
		// wait for tests to complete
		1.0.wait;
	}
}


