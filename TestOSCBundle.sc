
TestOSCBundle : UnitTest {
	
	test_prepare {
		var sd, b,completed=false;

		b = OSCBundle.new;

		sd = Array.fill(100,{ |i|
				var d;
				d = SynthDef( "TestOSCBundle" ++ i,{
					SinOsc.ar(400 + i)
				});
				
				b.addPrepare( ["/d_recv", d.asBytes] )

			});
		
		this.bootServer;
		
		b.doPrepare(Server.default,{ completed = true});
		
		this.asynchAssert( {completed}, {
			this.assert( completed, "should be completed")
		},"waiting for prepare to send all");

	}

}


