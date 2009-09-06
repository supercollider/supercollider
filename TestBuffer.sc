/*
Buffer.test
UnitTest.gui
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
	
	
	// Test basen on JP's test code, for the bug which DS introduced in svn rev 9362, and JP fixed in svn rev 9371
	// Note that the "expected values" used in this test depend precisely on the samples in a11wlk01.wav, so will need updating if that changes.
	test_allocAndQuery {
		var o,p, expectq, expectg, s=this.s, sfpath, sf, b;
		this.bootServer;
		Buffer.freeAll;
		
		o = OSCresponderNode(s.addr, '/b_info', {arg time, resp, msg;
			msg.postln;
			this.assertEquals(msg, expectq, "/b_info data returned from a /b_query message should be as expected");
		});
			
		p = OSCresponderNode(s.addr, '/b_set', {arg time, resp, msg;
			msg.postln;
			this.assertEquals(msg, expectg, "/b_set data returned from a /b_get message should be as expected");
		});
			
		o.add;
		p.add;
		
		// Hmm, seems not all systems have the standard soundfile available
		// s.sendMsg(\b_allocRead, 1, "sounds/a11wlk01.wav");
		// So instead we use this from the SoundFile helpfile:
		sf = SoundFile.new.headerFormat_("AIFF").sampleFormat_("int16").numChannels_(1);
		sfpath = PathName.tmp +/+ "sc3_test_allocAndQuery.aiff";
		sf.openWrite(sfpath);
		// sawtooth
		b = Signal.sineFill(441, (1..20).reciprocal);
		// write multiple cycles (441 * 100 = 1 sec worth)
		100.do{ sf.writeData(b) };
		sf.close;
		0.1.wait;
		s.sendMsg(\b_allocRead, 1, sfpath);

		0.4.wait;
		s.sync;
		
		expectq = [ '/b_info', 1, 44100, 1, 44100 ];
		expectg = [ '/b_set', 1, 1000, 0.43011474609375 ];
		s.sendMsg(\b_query, 1);
		s.sendMsg(\b_get, 1, 1000);
		
		0.4.wait;
		s.sync;
		
		s.sendMsg(\b_alloc, 3, 30000);
		s.sendMsg(\b_read, 3, sfpath, 0, 30000);
		
		0.4.wait;
		s.sync;
		
		expectq = [ '/b_info', 3, 30000, 1, 44100 ];
		expectg = [ '/b_set', 3, 1000, 0.43011474609375 ];
		s.sendMsg(\b_query, 3);
		s.sendMsg(\b_get, 3, 1000);
		
		0.4.wait;
		s.sync;
		
		o.remove;
		p.remove;
		
		
	}
	
}


