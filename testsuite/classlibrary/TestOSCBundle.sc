
TestOSCBundle : UnitTest {
	
	test_prepare {
		var sd, b,completed=false;

		b = OSCBundle.new;

		sd = Array.fill(100,{ |i|
				var d;
				d = SynthDef( "TestOSCBundle" ++ i,{
					SinOsc.ar(400 + i,mul: 0.0001)
				});
				
				b.addPrepare( ["/d_recv", d.asBytes] )

			});
		
		this.bootServer;
		
		b.doPrepare(Server.default,{ completed = true});
		
		this.wait( {completed},"waiting for prepare to send all");
		
	}

}


