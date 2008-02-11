
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
}

