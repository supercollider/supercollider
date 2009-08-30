/*
Buffer.test
*/
TestBuffer : UnitTest {
	
	test_freeAll {
		var n;
		
		this.bootServer;
		
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
		Server.default.sync;
		b.free;
		Server.default.sync;
		// load then load
		b = Buffer.loadCollection(Server.default, d);
		Server.default.sync;
		b.loadToFloatArray(0, -1, action:{|data| 
			this.assertArrayFloatEquals(data - d, 0, report: true)
		});
		Server.default.sync;
		// zero the Buffer and check that it worked
		b.zero;
		Server.default.sync;
		b.loadToFloatArray(0, -1, action:{|data| 
			this.assertArrayFloatEquals(data, 0, "Buffer:zero should give a buffer containing zeroes", report: true)
		});
		Server.default.sync;
		b.free;
		Server.default.sync;
	}
}


