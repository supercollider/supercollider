
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
	test_copy {
		var z,w;
		
		z = ControlSpec(50,100);
		this.assert(z.warp.spec === z,"the spec's warp's spec should be the spec");

		w = z.copy;
		this.assert(z.warp !== w.warp,"should not copy the warp object when you copy a control spec");
		this.assert( w.warp.spec === w,"the copied spec's warp should be the copied spec");

		z.minval = 70;
		this.assert(z.minval == 70,"setting minval on spec should work");

		this.assert( w.minval != 70,"setting minval on spec should not affect its copy");
		
		// and here is the bug itself
		this.assert( w.map(0.0) == 50,"mapping using the copied spec should result in the original mapping behavior of the original spec.");
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

