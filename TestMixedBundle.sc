
TestMixedBundle : UnitTest {

	var t;
	setUp {
		this.bootServer;
		t = MixedBundle.new;
	}

	// helper method, not a test
	makeDefs { |numDefs|
		numDefs.do({ |i|
			var d;
			d = SynthDef( "TestOSCBundle" ++ i,{
					SinOsc.ar([400,403] + i)
			});

			t.addPrepare( ["/d_recv", d.asBytes] )

		});
	}

	test_prepare { arg numDefs = 100;
		var functionFired = false, sent;

		this.makeDefs(numDefs);
		t.addFunction({ functionFired = true });

		t.send(Server.default);
		this.wait( { functionFired }, "wait for functionFired to be set by bundle.doFunction");

	}

// crashes the language !
//	test_largePrepare {
//		this.test_prepare(1000)
//	}


	test_send {
		var functionFired = false, sent,onSendFired = false;
		this.makeDefs(1);

		// defs should be loaded
		t.add( Synth.basicNew("TestOSCBundle" ++ 0).newMsg );

		t.addFunction({ functionFired = true });
		t.onSend({ onSendFired = true });

		t.send(Server.default);
		this.wait( { functionFired }, "wait for functionFired to be set by bundle.doFunction");

		this.assert( onSendFired,"onSendFired should be set to true");

		// now we want something that can catch all failures back from the server
		2.0.wait;
		if(Server.default.aliveThreadIsRunning.not,{
			"Server is not running alive thread, unit test cannot check numSynths".die;
		});
		// one synth should be playing
		this.assertEquals( Server.default.numSynths, 1, "should be one synth playing");
	}

	test_mediumSend {
		var functionFired = false, sent;
		this.makeDefs(100);

		// defs should be loaded
		t.add( Synth.basicNew("TestOSCBundle" ++ 0).newMsg );

		t.addFunction({ functionFired = true });


		t.send(Server.default);
		this.wait( { functionFired }, "wait for functionFired to be set by bundle.doFunction");

		// now we want something that can catch all failures back from the server
		2.0.wait;
		if(Server.default.aliveThreadIsRunning.not,{
			"Server is not running alive thread, unit test cannot check numSynths".die;
		});
		// one synth should be playing
		this.assertEquals( Server.default.numSynths, 1, "should be one synth playing");
	}
	
}


