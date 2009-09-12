/*
Env.test
UnitTest.gui
*/
TestEnv : UnitTest {
	
	test_equality {
		var a,b,c;
		a = Env([ 0, 1.0, 1.0, 0 ], [ 0.01, 1.0, 1.0 ], 'lin', nil, nil);

		b = Env([ 0, 1.0, 1.0, 0 ], [ 0.01, 1.0, 1.0 ], 'lin', nil, nil);
		c = Env([ 0, 2.0, 1.0, 0 ], [ 0.01, 1.0, 1.0 ], 'lin', nil, nil);

		this.assert( a == b, "2 identically specified Envs should be equal");

		this.assert( a != c, "2 different Envs should not be equal");
		
		this.assert( a == a, "an Env should equal itself");
	}
	
	test_equivalences {
	
		this.assertArrayFloatEquals(
			Env.cutoff(1, 1, 'exp').asSignal(20).as(Array),
			Env.cutoff(1, 1, 'lin').asSignal(20).asArray.linexp(0,1,-100.dbamp,1),
			"Exponential Env.cutoff should be same as linear Env.cutoff.linexp");
	}
}

