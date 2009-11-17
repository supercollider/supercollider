/*

TestNoOps.run
UnitTest.gui

*/

TestNoOps : UnitTest {

	test_array {
		var ops, check, n = 300;
		var a, b, r1, r2;
		
		
		check = { |func, inval|
			func.value(func.value(inval)) == inval
		};
		
		a = Array.rand(300, 0, 100);
		b = a.clump(10);
		
		r1 = b.size.rand;
		r2 = b.size.rand;
		
		this.assert(n.even, "for this test, array must have even number of elements");
		
		ops = (
			\reverse: { |x| x.reverse },
			\rotate: { |x| x.rotate(x.size div: 2) },
			\swap: {|x| x.swap(r1, r2) },
			\flop: {|x| x.flop }
		);
		
		ops.keysValuesDo { |name, func|
			this.assert(check.value(func, b), "the operation '%' should be no-op when applied twice".format(name))
		};
		
		
		this.assert(b.flat == a, "flat should return a flat array of the same order");
		this.assert(b.flatten(1) == a, "flatten should return a flat array of the same order");
		this.assert(b.sum.sum == a.sum, "summing should be invariant with sub arrays");
		this.assert({ |...args| args }.flop.value(*b) == b.flop, 
			"function flop should behave in analogy to array.flop");

		
			
		
	} // End test_array_no_ops
	
	
	
}