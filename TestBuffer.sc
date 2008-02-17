
TestBuffer : UnitTest {
	
	test_freeAll {
		var n;
		Buffer.freeAll;
		n = Buffer(Server.default,44100,2);
		this.assertEquals( Server.default.bufferAllocator.blocks.size , 1 , " allocated one buffer");
		Buffer.freeAll;
		this.assertEquals( Server.default.bufferAllocator.blocks.size , 0 , "freeAll : no buffers allocated");
		
	}
	
}


