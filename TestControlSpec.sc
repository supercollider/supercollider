
TestControlSpec : UnitTest {
	
	test_equals {
		var a,b,c;
		a = ControlSpec(0, 1, 'linear', 0, 0.0, "");
		b = ControlSpec(0, 1, 'linear', 0, 0.0, "");
		c = ControlSpec(0, 2, 'linear', 0, 0.0, "");
		
		// no really, this failed !
		this.assert( a == b, "2 identical control specs should be equal");
		this.assert( a != c, "2 different control specs should not be equal");
		
		this.assert( a == a, "control spec should equal itself");

	}
	
}

TestCurveWarp :UnitTest {
	
	test_equals {
		var s,c,d,e;
		s = ControlSpec(0.3,10.0);
		c = CurveWarp(s,-2);
		d = CurveWarp(s,-2);
		e = CurveWarp(s,-3);
		
		this.assert( c == d, "CurveWarp should be equal");
		this.assert( c != e, "CurveWarp should not be equal");
		this.assert( c == c, "identical CurveWarp should be equal");
		this.assert( c != "rabbits", "CurveWarp should not equal rabbits");
	}
}

